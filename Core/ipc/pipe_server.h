#pragma once

namespace seebot::ipc
{
class PipeServer
{
public:
    bool Start();
    void Stop();
};
} // namespace seebot::ipc
