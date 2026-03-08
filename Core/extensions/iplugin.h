#pragma once

namespace seebot::extensions
{
class IPlugin
{
public:
    virtual ~IPlugin() = default;
    virtual void OnLoad() = 0;
    virtual void OnUnload() = 0;
};
} // namespace seebot::extensions
