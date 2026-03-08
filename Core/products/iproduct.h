#pragma once

namespace seebot::products
{
class IProduct
{
public:
    virtual ~IProduct() = default;
    virtual void Tick() = 0;
};
} // namespace seebot::products
