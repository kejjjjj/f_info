

#include <windows.h>
#include <shlobj.h>
#include <string>

// Function to open Command Prompt and print the selected file name
void OpenCmdWithSelectedFile(const std::wstring& filePath)
{
    std::wstring command = L"cmd.exe /c echo " + filePath;
    ShellExecuteW(NULL, L"open", L"cmd.exe", command.c_str(), NULL, SW_SHOW);
}

const CLSID CLSID_HelloContextMenu = { 0x3213BA07, 0x4E9D, 0x48B9, 0xABDA, 0x381BD6AC9510 };

// Context Menu Handler class
class HelloContextMenuHandler : public IContextMenu
{
private:
    ULONG m_refCount;

public:
    HelloContextMenuHandler() : m_refCount(1) {}

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IContextMenu))
        {
            *ppv = static_cast<IContextMenu*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&m_refCount); }

    STDMETHODIMP_(ULONG) Release()
    {
        ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0)
            delete this;
        return refCount;
    }

    // IContextMenu methods
    STDMETHODIMP QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
    {
        if (uFlags & CMF_DEFAULTONLY)
            return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

        InsertMenuW(hmenu, indexMenu, MF_STRING | MF_BYPOSITION, idCmdFirst, L"Copy Data");
        return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 1);
    }

    STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO lpici)
    {
        if (HIWORD(lpici->lpVerb))
            return E_INVALIDARG;

        std::wstring filePath = reinterpret_cast<LPCWSTR>(lpici->lpVerb);

        OpenCmdWithSelectedFile(filePath);

        return S_OK;
    }

    STDMETHODIMP GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
    {
        if (uFlags & GCS_HELPTEXT)
        {
            if (idCmd == 0)
            {
                lstrcpynA(pszName, "Print selected file name", cchMax);
                return S_OK;
            }
        }

        return E_INVALIDARG;
    }
};

// DLL Entry Point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

// Entry point for creating the context menu handler
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    *ppv = NULL;
    if (IsEqualIID(rclsid, CLSID_HelloContextMenu))
    {
        HelloContextMenuHandler* pHandler = new HelloContextMenuHandler();
        if (pHandler == NULL)
            return E_OUTOFMEMORY;

        return pHandler->QueryInterface(riid, ppv);
    }

    return CLASS_E_CLASSNOTAVAILABLE;
}

// Entry point for registering the context menu handler
extern "C" __declspec(dllexport) long DllRegisterServer()
{
    MessageBoxA(NULL, "HUH", "LOOOL", MB_ICONSTOP);

    HKEY hKey;
    LONG lResult = RegCreateKeyW(HKEY_CLASSES_ROOT, L"*\\shell\\copy_data", &hKey);
    if (lResult != ERROR_SUCCESS)
        return HRESULT_FROM_WIN32(lResult);

    WCHAR szModule[MAX_PATH];
    if (GetModuleFileNameW(NULL, szModule, ARRAYSIZE(szModule)) == 0)
        return HRESULT_FROM_WIN32(GetLastError());

    std::wstring command = std::wstring(szModule) + L" \"%1\"";
    lResult = RegSetValueExW(hKey, NULL, 0, REG_SZ, (const BYTE*)L"copy_data", sizeof(L"copy_data"));
    if (lResult != ERROR_SUCCESS)
        return HRESULT_FROM_WIN32(lResult);

    HKEY hCommandKey;
    lResult = RegCreateKeyW(hKey, L"command", &hCommandKey);
    if (lResult != ERROR_SUCCESS)
        return HRESULT_FROM_WIN32(lResult);

    lResult = RegSetValueExW(hCommandKey, NULL, 0, REG_SZ, (const BYTE*)command.c_str(), (DWORD)((command.size() + 1) * sizeof(wchar_t)));
    if (lResult != ERROR_SUCCESS)
        return HRESULT_FROM_WIN32(lResult);

    RegCloseKey(hCommandKey);
    RegCloseKey(hKey);

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);

    return S_OK;
}

// Entry point for unregistering the context menu handler
STDAPI DllUnregisterServer()
{
    RegDeleteTreeW(HKEY_CLASSES_ROOT, L"*\\shell\\Hello");
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
    return S_OK;
}