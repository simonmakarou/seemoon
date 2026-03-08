#pragma once

#include "iproduct.h"

namespace seebot::products
{
class Gatherer : public IProduct
{
public:
    void Tick() override;
};
} // namespace seebot::products
