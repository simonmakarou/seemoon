#include <windows.h>
#include <tlhelp32.h>

#include <cstring>
#include <iostream>
#include <string>

namespace {

void PrintLastError(const std::string& context) {
    const DWORD error = GetLastError();
    std::cerr << "[!] " << context << " failed. GetLastError=" << error << std::endl;
}

DWORD FindProcess(const char* processName) {
    std::cout << "[*] Creating process snapshot..." << std::endl;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        PrintLastError("CreateToolhelp32Snapshot");
        return 0;
    }

    PROCESSENTRY32A entry{};
    entry.dwSize = sizeof(entry);

    std::cout << "[*] Enumerating processes to find " << processName << "..." << std::endl;
    if (!Process32FirstA(snapshot, &entry)) {
        PrintLastError("Process32FirstA");
        CloseHandle(snapshot);
        return 0;
    }

    do {
        std::cout << "    PID=" << entry.th32ProcessID << " EXE=" << entry.szExeFile << std::endl;
        if (_stricmp(entry.szExeFile, processName) == 0) {
            std::cout << "[+] Target process found: PID=" << entry.th32ProcessID << std::endl;
            CloseHandle(snapshot);
            return entry.th32ProcessID;
        }
    } while (Process32NextA(snapshot, &entry));

    std::cout << "[-] Target process not found." << std::endl;
    CloseHandle(snapshot);
    return 0;
}

}  // namespace

int main(int argc, char* argv[]) {
    std::cout << "=== DLL Injector (Win32, C++17) ===" << std::endl;

    if (argc < 2) {
        std::cerr << "Usage: Injector.exe <full_path_to_dll>" << std::endl;
        return 1;
    }

    const char* processName = "Wow.exe";
    const char* dllPath = argv[1];
    const SIZE_T dllPathSize = std::strlen(dllPath) + 1;

    std::cout << "[*] Target process name: " << processName << std::endl;
    std::cout << "[*] DLL path: " << dllPath << std::endl;

    const DWORD pid = FindProcess(processName);
    if (pid == 0) {
        std::cerr << "[!] Unable to continue without target PID." << std::endl;
        return 1;
    }

    std::cout << "[*] Opening process with PROCESS_ALL_ACCESS..." << std::endl;
    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (process == nullptr) {
        PrintLastError("OpenProcess");
        return 1;
    }

    std::cout << "[*] Allocating memory in target process for DLL path..." << std::endl;
    LPVOID remoteBuffer = VirtualAllocEx(process, nullptr, dllPathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (remoteBuffer == nullptr) {
        PrintLastError("VirtualAllocEx");
        CloseHandle(process);
        return 1;
    }

    std::cout << "[*] Writing DLL path to remote process memory..." << std::endl;
    SIZE_T bytesWritten = 0;
    if (!WriteProcessMemory(process, remoteBuffer, dllPath, dllPathSize, &bytesWritten) || bytesWritten != dllPathSize) {
        PrintLastError("WriteProcessMemory");
        VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(process);
        return 1;
    }
    std::cout << "[+] Wrote " << bytesWritten << " bytes to remote process." << std::endl;

    std::cout << "[*] Resolving LoadLibraryA from kernel32.dll..." << std::endl;
    HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
    if (kernel32 == nullptr) {
        PrintLastError("GetModuleHandleA(kernel32.dll)");
        VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(process);
        return 1;
    }

    FARPROC loadLibraryA = GetProcAddress(kernel32, "LoadLibraryA");
    if (loadLibraryA == nullptr) {
        PrintLastError("GetProcAddress(LoadLibraryA)");
        VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(process);
        return 1;
    }

    std::cout << "[*] Creating remote thread (entrypoint=LoadLibraryA)..." << std::endl;
    HANDLE remoteThread = CreateRemoteThread(
        process,
        nullptr,
        0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibraryA),
        remoteBuffer,
        0,
        nullptr
    );

    if (remoteThread == nullptr) {
        PrintLastError("CreateRemoteThread");
        VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(process);
        return 1;
    }

    std::cout << "[*] Waiting for remote thread to finish..." << std::endl;
    const DWORD waitResult = WaitForSingleObject(remoteThread, INFINITE);
    if (waitResult != WAIT_OBJECT_0) {
        PrintLastError("WaitForSingleObject");
        CloseHandle(remoteThread);
        VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(process);
        return 1;
    }

    DWORD threadExitCode = 0;
    if (!GetExitCodeThread(remoteThread, &threadExitCode)) {
        PrintLastError("GetExitCodeThread");
        CloseHandle(remoteThread);
        VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(process);
        return 1;
    }

    std::cout << "[+] Remote thread completed. Exit code (HMODULE): 0x" << std::hex << threadExitCode << std::dec << std::endl;

    std::cout << "[*] Releasing resources..." << std::endl;
    if (!VirtualFreeEx(process, remoteBuffer, 0, MEM_RELEASE)) {
        PrintLastError("VirtualFreeEx");
    }

    CloseHandle(remoteThread);
    CloseHandle(process);

    std::cout << "[+] Injection flow finished." << std::endl;
    return 0;
}
