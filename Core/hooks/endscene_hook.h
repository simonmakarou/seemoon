#pragma once

namespace seebot::hooks
{
enum class EndSceneHookState
{
    not_installed,
    installing,
    installed,
    failed,
};

enum class EndSceneHookResult
{
    success,
    already_installed,
    install_error,
    remove_error,
};

using EndSceneFunction = void (*)();

EndSceneHookResult InstallEndSceneHook();
EndSceneHookResult RemoveEndSceneHook();

[[nodiscard]] EndSceneHookState GetEndSceneHookState();
[[nodiscard]] EndSceneFunction GetOriginalEndScene();
} // namespace seebot::hooks
