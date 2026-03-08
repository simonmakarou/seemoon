#include "core.h"

#include <windows.h>

#include "hooks/endscene_hook.h"

namespace seebot
{
namespace
{
const char* ToDebugString(const hooks::EndSceneHookResult result)
{
    switch (result)
    {
    case hooks::EndSceneHookResult::success:
        return "success";
    case hooks::EndSceneHookResult::already_installed:
        return "already_installed";
    case hooks::EndSceneHookResult::install_error:
        return "install_error";
    case hooks::EndSceneHookResult::remove_error:
        return "remove_error";
    default:
        return "unknown";
    }
}
} // namespace

void Init()
{
    const hooks::EndSceneHookResult result = hooks::InstallEndSceneHook();
    OutputDebugStringA(ToDebugString(result));
}

void MainLoop()
{
}

void Shutdown()
{
    const hooks::EndSceneHookResult result = hooks::RemoveEndSceneHook();
    OutputDebugStringA(ToDebugString(result));
}
} // namespace seebot
