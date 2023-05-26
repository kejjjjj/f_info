
#include "reg_helpers.hpp"

// Function to open Command Prompt and print the selected file name
void OpenCmdWithSelectedFile(const std::wstring& filePath)
{
    std::wstring command = L"cmd.exe /c echo " + filePath;
    ShellExecuteW(NULL, L"open", L"cmd.exe", command.c_str(), NULL, SW_SHOW);
}
//{3213BA07-4E9D-48B9-ABDA-381BD6AC9510}
// {1137CA2D-6141-4126-8302-0FBE0619B3CA}
static const GUID CLSID_HelloContextMenu =
{ 0x1137ca2d, 0x6141, 0x4126, { 0x83, 0x2, 0xf, 0xbe, 0x6, 0x19, 0xb3, 0xca } };

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
        MessageBoxA(NULL, "QueryInterface", "ye", MB_ICONSTOP);

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
        MessageBoxA(NULL, "QueryContextMenu", "ye", MB_ICONSTOP);

        if (uFlags & CMF_DEFAULTONLY)
            return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

        InsertMenuW(hmenu, indexMenu, MF_STRING | MF_BYPOSITION, idCmdFirst, L"Copy Data");
        return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 1);
    }

    STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO lpici)
    {
        MessageBoxA(NULL, "InvokeCommand", "ye", MB_ICONSTOP);

        if (HIWORD(lpici->lpVerb))
            return E_INVALIDARG;

        std::wstring filePath = reinterpret_cast<LPCWSTR>(lpici->lpVerb);

        OpenCmdWithSelectedFile(filePath);

        return S_OK;
    }

    STDMETHODIMP GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
    {
        MessageBoxA(NULL, "GetCommandString", "ye", MB_ICONSTOP);

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
_Check_return_ STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID FAR* ppv)
{
    MessageBoxA(NULL, "DllGetClassObject", "ye", MB_ICONSTOP);

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
STDAPI DllRegisterServer()
{
    //C:\Users\luukas230\source\repos\f_info\x64\Release\f_info.exe "%1"
    HKEY hKey;
    HKEY shell;
    hKey = NewRegistryKey(HKEY_CLASSES_ROOT, L"*\\shell\\copy_data");

    NewRegistryString(hKey, L"MUIVerb", L"Datagrabber"); //identifier name in the menu
    NewRegistryString(hKey, L"SubCommands", L""); //the key needs to be empty


    shell = NewRegistryKey(hKey, L"shell");
    hKey = NewRegistryKey(shell, L"copy_to_clipboard");
    
    NewRegistryString(hKey, L"MUIVerb", L"Copy To Clipboard"); //identifier name in the menu
    hKey = NewRegistryKey(hKey, L"command");
    EditRegistryStringDefault(hKey, L"C:\\Users\\luukas230\\source\\repos\\f_info\\x64\\Release\\f_info.exe \"%1 /default\""); //identifier name in the menu

    hKey = shell;

    hKey = NewRegistryKey(shell, L"get_md5_hash");

    NewRegistryString(hKey, L"MUIVerb", L"Copy MD5"); //identifier name in the menu
    hKey = NewRegistryKey(hKey, L"command");
    EditRegistryStringDefault(hKey, L"C:\\Users\\luukas230\\source\\repos\\f_info\\x64\\Release\\f_info.exe \"%1 -md5\""); //identifier name in the menu

    RegCloseKey(hKey);
    RegCloseKey(shell);

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);

    return S_OK;
}

// Entry point for unregistering the context menu handler
STDAPI DllUnregisterServer()
{
    RegDeleteTreeW(HKEY_CLASSES_ROOT, L"*\\shell\\copy_data");
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
    return S_OK;
}