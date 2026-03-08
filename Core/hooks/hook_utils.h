#pragma once

#include "endscene_hook.h"

namespace seebot::hooks
{
inline bool IsHookInstalled()
{
    return GetEndSceneHookState() == EndSceneHookState::installed;
}
} // namespace seebot::hooks
