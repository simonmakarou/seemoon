#include "lua_engine.h"

namespace seebot::game
{
void LuaEngine::Execute(const char*)
{
    // Lua должен вызываться только из игрового потока (через будущий EndScene hook).
}
} // namespace seebot::game
