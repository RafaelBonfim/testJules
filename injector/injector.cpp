#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <vector>
#include <algorithm>

DWORD GetProcessIdByName(const std::wstring& processName) {
    PROCESSENTRY32W processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32W);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }
    if (Process32FirstW(snapshot, &processEntry)) {
        do {
            if (_wcsicmp(processEntry.szExeFile, processName.c_str()) == 0) {
                CloseHandle(snapshot);
                return processEntry.th32ProcessID;
            }
        } while (Process32NextW(snapshot, &processEntry));
    }
    CloseHandle(snapshot);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <ProcessName.exe>" << std::endl;
        return 1;
    }

    std::string processNameStr(argv[1]);
    std::wstring processName(processNameStr.begin(), processNameStr.end());

    // Get the full path to the DLL
    char dllPath[MAX_PATH];
    if (GetFullPathNameA("dx11hook.dll", MAX_PATH, dllPath, NULL) == 0) {
        std::cerr << "Error getting full path to DLL. Error code: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "Attempting to inject: " << dllPath << std::endl;

    DWORD procID = GetProcessIdByName(processName);
    if (procID == 0) {
        std::cerr << "Process '" << processNameStr << "' not found." << std::endl;
        return 1;
    }

    std::cout << "Target process found. PID: " << procID << std::endl;

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
    if (hProcess == NULL) {
        std::cerr << "Failed to open target process. Error code: " << GetLastError() << std::endl;
        return 1;
    }
    std::cout << "Handle to process obtained." << std::endl;

    // Allocate memory for the DLL path in the target process
    LPVOID pDllPath = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (pDllPath == NULL) {
        std::cerr << "Failed to allocate memory in target process. Error code: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return 1;
    }
    std::cout << "Memory allocated in target process." << std::endl;

    // Write the DLL path to the allocated memory
    if (!WriteProcessMemory(hProcess, pDllPath, dllPath, strlen(dllPath) + 1, NULL)) {
        std::cerr << "Failed to write DLL path to target process. Error code: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }
    std::cout << "DLL path written to target process." << std::endl;

    // Create a remote thread to load the DLL
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, pDllPath, 0, NULL);
    if (hThread == NULL) {
        std::cerr << "Failed to create remote thread. Error code: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    std::cout << "Injection successful! Waiting for thread to finish..." << std::endl;

    WaitForSingleObject(hThread, INFINITE);

    std::cout << "Remote thread finished." << std::endl;

    // Clean up
    VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return 0;
}
