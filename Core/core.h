#pragma once

namespace seebot
{
struct CoreLifecycleState
{
    bool bootstrap_started;
    bool runtime_ready;
    bool shutdown_requested;
    bool shutdown_done;
};

void Init();
void OnFrameTick();
void Shutdown();

[[nodiscard]] CoreLifecycleState GetCoreLifecycleState();
} // namespace seebot
