add_library(SeeBot SHARED
    core.cpp
    dllmain.cpp
    hooks/endscene_hook.cpp
    game/inventory.cpp
    game/lua_engine.cpp
    game/movement.cpp
    game/spell_manager.cpp
    game/target_manager.cpp
    ipc/pipe_server.cpp
    objects/object_manager.cpp
    products/gatherer.cpp
)

target_include_directories(SeeBot PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(SeeBot PROPERTIES
    OUTPUT_NAME "SeeBot"
)
