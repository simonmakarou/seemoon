#include "endscene_hook.h"

#include <mutex>

namespace seebot::hooks
{
namespace
{
std::mutex g_endSceneHookMutex;
EndSceneHookState g_endSceneHookState = EndSceneHookState::not_installed;
EndSceneFunction g_endSceneTarget = nullptr;
EndSceneFunction g_originalEndScene = nullptr;
bool g_hookEnabled = false;
EndSceneTickCallback g_tickCallback = nullptr;

void EndSceneHookStub()
{
    if (g_tickCallback != nullptr)
    {
        g_tickCallback();
    }
}

bool ResolveEndSceneTarget(EndSceneFunction& target)
{
    // TODO: заменить на реальный поиск адреса EndScene.
    target = &EndSceneHookStub;
    return target != nullptr;
}

bool ValidateEndSceneTarget(const EndSceneFunction target)
{
    return target != nullptr;
}

bool CreateEndSceneHook(const EndSceneFunction target, EndSceneFunction& original)
{
    // TODO: заменить на вызов backend-хукера (например, MinHook::CreateHook).
    if (target == nullptr)
    {
        return false;
    }

    original = target;
    return true;
}

bool EnableEndSceneHook(const EndSceneFunction target)
{
    // TODO: заменить на backend enable.
    return target != nullptr;
}

bool DisableEndSceneHook(const EndSceneFunction target)
{
    // TODO: заменить на backend disable.
    return target != nullptr;
}

bool RemoveEndSceneHookInternal(const EndSceneFunction target)
{
    // TODO: заменить на backend remove.
    return target != nullptr;
}
} // namespace


void SetEndSceneTickCallback(const EndSceneTickCallback callback)
{
    std::lock_guard<std::mutex> lock(g_endSceneHookMutex);
    g_tickCallback = callback;
}

EndSceneHookResult InstallEndSceneHook()
{
    std::lock_guard<std::mutex> lock(g_endSceneHookMutex);

    if (g_endSceneHookState == EndSceneHookState::installed)
    {
        return EndSceneHookResult::already_installed;
    }

    g_endSceneHookState = EndSceneHookState::installing;

    EndSceneFunction target = nullptr;
    bool hookCreated = false;
    bool hookEnabled = false;

    if (!ResolveEndSceneTarget(target) || !ValidateEndSceneTarget(target))
    {
        g_endSceneHookState = EndSceneHookState::failed;
        return EndSceneHookResult::install_error;
    }

    g_endSceneTarget = target;

    if (!CreateEndSceneHook(g_endSceneTarget, g_originalEndScene))
    {
        g_endSceneTarget = nullptr;
        g_originalEndScene = nullptr;
        g_endSceneHookState = EndSceneHookState::failed;
        return EndSceneHookResult::install_error;
    }
    hookCreated = true;

    if (!EnableEndSceneHook(g_endSceneTarget))
    {
        if (hookCreated)
        {
            RemoveEndSceneHookInternal(g_endSceneTarget);
        }
        g_endSceneTarget = nullptr;
        g_originalEndScene = nullptr;
        g_endSceneHookState = EndSceneHookState::failed;
        return EndSceneHookResult::install_error;
    }
    hookEnabled = true;

    g_hookEnabled = hookEnabled;
    g_endSceneHookState = EndSceneHookState::installed;
    return EndSceneHookResult::success;
}

EndSceneHookResult RemoveEndSceneHook()
{
    std::lock_guard<std::mutex> lock(g_endSceneHookMutex);

    if (g_endSceneHookState != EndSceneHookState::installed)
    {
        g_endSceneTarget = nullptr;
        g_originalEndScene = nullptr;
        g_hookEnabled = false;
        if (g_endSceneHookState != EndSceneHookState::installing)
        {
            g_endSceneHookState = EndSceneHookState::not_installed;
        }
        return EndSceneHookResult::success;
    }

    if (g_hookEnabled && !DisableEndSceneHook(g_endSceneTarget))
    {
        g_endSceneHookState = EndSceneHookState::failed;
        return EndSceneHookResult::remove_error;
    }

    g_hookEnabled = false;

    if (!RemoveEndSceneHookInternal(g_endSceneTarget))
    {
        g_endSceneHookState = EndSceneHookState::failed;
        return EndSceneHookResult::remove_error;
    }

    g_endSceneTarget = nullptr;
    g_originalEndScene = nullptr;
    g_endSceneHookState = EndSceneHookState::not_installed;
    return EndSceneHookResult::success;
}

EndSceneHookState GetEndSceneHookState()
{
    std::lock_guard<std::mutex> lock(g_endSceneHookMutex);
    return g_endSceneHookState;
}

EndSceneFunction GetOriginalEndScene()
{
    std::lock_guard<std::mutex> lock(g_endSceneHookMutex);
    return g_originalEndScene;
}
} // namespace seebot::hooks
