#pragma once

namespace seebot::extensions
{
class IRotation
{
public:
    virtual ~IRotation() = default;
    virtual void Execute() = 0;
};
} // namespace seebot::extensions
