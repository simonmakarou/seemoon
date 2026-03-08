#pragma once

#include <cstdint>

namespace seebot::objects
{
class WoWObject
{
public:
    virtual ~WoWObject() = default;
    std::uint64_t Guid() const { return guid_; }

protected:
    std::uint64_t guid_ = 0;
};
} // namespace seebot::objects
