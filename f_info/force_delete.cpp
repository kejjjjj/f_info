
#include "pch.hpp"

void force_delete_file(const std::string& filepath)
{
    DWORD pid = get_file_host_pid(filepath);

    std::cout << "process pid: " << pid << '\n';

    system("pause");
    std::string name = get_file_host_name(pid);

    std::cout << "name: " << name << '\n';
    system("pause");
}
typedef NTSTATUS(NTAPI* PNtQueryInformationProcess)(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
    );

DWORD GetProcessIdFromHandle(HANDLE handle) {
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    if (ntdll == NULL) {
        throw std::exception("failed to get handle to ntdll.dll");
        return 0;
    }

    PNtQueryInformationProcess ntQueryInformationProcess = (PNtQueryInformationProcess)GetProcAddress(ntdll, "NtQueryInformationProcess");

    if (ntQueryInformationProcess == NULL) {
        throw std::exception("failed to get process information: GetProcAddress");
        return 0;
    }

    PROCESS_BASIC_INFORMATION processInfo;
    ULONG returnLength;

    NTSTATUS status = ntQueryInformationProcess(handle, ProcessBasicInformation, &processInfo, sizeof(processInfo), &returnLength);
    if (returnLength == NULL) {

        throw std::exception(std::format("failed to query process information:\n{}", returnLength).c_str());
        return 0;
    }

    return (DWORD)processInfo.UniqueProcessId;
}
//HANDLE GetProcessHandleByName(const std::string& processName) {
//    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//    if (snapshot == INVALID_HANDLE_VALUE) {
//        // Failed to create the process snapshot
//        // Handle the error
//        return nullptr;
//    }
//
//    PROCESSENTRY32 processEntry;
//    processEntry.dwSize = sizeof(PROCESSENTRY32);
//
//    if (Process32First(snapshot, &processEntry)) {
//        do {
//            std::string currentProcessName(processEntry.szExeFile);
//            if (currentProcessName == processName) {
//                HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processEntry.th32ProcessID);
//                CloseHandle(snapshot);
//                return processHandle;
//            }
//        } while (Process32Next(snapshot, &processEntry));
//    }
//
//    CloseHandle(snapshot);
//    return INVALID_HANDLE_VALUE;
//}

DWORD get_file_host_pid(const std::string& filepath)
{
    SECURITY_ATTRIBUTES a;


    HANDLE fileHandle = CreateFileA(filepath.c_str(), GENERIC_READ, (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE),NULL , OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL ,NULL);

    //auto fileHandle = GetProcessHandleByName("iw3mp.exe");
    //DWORD fileHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());


    if (fileHandle == INVALID_HANDLE_VALUE) {
        throw std::exception(std::format("unable to find who uses the file:\n{}\n:(", filepath).c_str());
        return 0;
    }

    DWORD PID = GetProcessIdFromHandle(fileHandle);

    if (PID == NULL) {
        throw std::exception(std::format("unable to get process id:\n{}\n:(", get_last_error()).c_str());
        return 0;
    }

    CloseHandle(fileHandle);

    return PID;

}
std::string get_file_host_name(DWORD pid)
{
    if (!pid)
        return "<invalid pid>";

    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        throw std::exception(std::format("unable to get handle snapshot:\n{}", get_last_error()).c_str());
        return "<invalid>";
    }

    bool found = false;

    if (Process32First(snapshot, &processEntry)) {
        do {
            if (processEntry.th32ProcessID == pid) {
                found = true;

                return processEntry.szExeFile;
            }
        } while (Process32Next(snapshot, &processEntry));
    }

    throw std::exception(std::format("unable to find process from list").c_str());
    return "<invalid>";
}