#include <windows.h>

#include "core.h"

namespace
{
DWORD WINAPI InitializationThread(LPVOID)
{
    MessageBoxA(nullptr, "Injected into WoW!", "SeeBot", MB_OK | MB_ICONINFORMATION);

    // Важно: Lua-вызовы должны выполняться только из игрового потока
    // (будущий EndScene hook), а не из DllMain/потока инициализации.
    seebot::Init();
    return 0;
}
} // namespace

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);

        HANDLE threadHandle = CreateThread(nullptr, 0, InitializationThread, nullptr, 0, nullptr);
        if (threadHandle != nullptr)
        {
            CloseHandle(threadHandle);
        }
    }

    return TRUE;
}
