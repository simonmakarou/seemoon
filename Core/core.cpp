#include "core.h"

#include <atomic>
#include <mutex>

#include <windows.h>

#include "hooks/endscene_hook.h"
#include "ipc/pipe_server.h"
#include "products/gatherer.h"

namespace seebot
{
namespace
{
std::mutex g_coreMutex;
CoreLifecycleState g_lifecycleState{false, false, false, false};

std::atomic<bool> g_allowFrameTicks{false};
std::atomic<unsigned int> g_activeTicks{0};

ipc::PipeServer g_pipeServer;
products::Gatherer g_gatherer;
bool g_pipeServerStarted = false;

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

void StopRuntimeProducts()
{
    // TODO: остановка всех продуктовых модулей, когда они будут объединены в менеджер.
    (void)g_gatherer;
}

void StopIpc()
{
    if (g_pipeServerStarted)
    {
        g_pipeServer.Stop();
        g_pipeServerStarted = false;
    }
}
} // namespace

void Init()
{
    std::lock_guard<std::mutex> lock(g_coreMutex);
    if (g_lifecycleState.runtime_ready || g_lifecycleState.shutdown_done)
    {
        return;
    }

    g_lifecycleState.bootstrap_started = true;
    g_lifecycleState.shutdown_requested = false;
    g_lifecycleState.shutdown_done = false;

    hooks::SetEndSceneTickCallback(&OnFrameTick);

    g_pipeServerStarted = g_pipeServer.Start();

    const hooks::EndSceneHookResult hookInstallResult = hooks::InstallEndSceneHook();
    OutputDebugStringA(ToDebugString(hookInstallResult));

    g_allowFrameTicks.store(hookInstallResult == hooks::EndSceneHookResult::success ||
                                hookInstallResult == hooks::EndSceneHookResult::already_installed,
                            std::memory_order_release);
    g_lifecycleState.runtime_ready = g_allowFrameTicks.load(std::memory_order_acquire);
}

void OnFrameTick()
{
    if (!g_allowFrameTicks.load(std::memory_order_acquire))
    {
        return;
    }

    g_activeTicks.fetch_add(1, std::memory_order_acq_rel);

    if (!g_allowFrameTicks.load(std::memory_order_acquire))
    {
        g_activeTicks.fetch_sub(1, std::memory_order_acq_rel);
        return;
    }

    g_gatherer.Tick();

    g_activeTicks.fetch_sub(1, std::memory_order_acq_rel);
}

void Shutdown()
{
    {
        std::lock_guard<std::mutex> lock(g_coreMutex);
        if (!g_lifecycleState.bootstrap_started || g_lifecycleState.shutdown_done || g_lifecycleState.shutdown_requested)
        {
            return;
        }

        g_lifecycleState.shutdown_requested = true;
        g_allowFrameTicks.store(false, std::memory_order_release);
    }

    while (g_activeTicks.load(std::memory_order_acquire) != 0)
    {
        Sleep(1);
    }

    StopIpc();
    StopRuntimeProducts();

    const hooks::EndSceneHookResult hookRemoveResult = hooks::RemoveEndSceneHook();
    OutputDebugStringA(ToDebugString(hookRemoveResult));

    hooks::SetEndSceneTickCallback(nullptr);

    std::lock_guard<std::mutex> lock(g_coreMutex);
    g_lifecycleState.runtime_ready = false;
    g_lifecycleState.shutdown_done = true;
}

CoreLifecycleState GetCoreLifecycleState()
{
    std::lock_guard<std::mutex> lock(g_coreMutex);
    return g_lifecycleState;
}
} // namespace seebot
