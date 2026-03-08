add_executable(Injector
    main.cpp
)

set_target_properties(Injector PROPERTIES
    WIN32_EXECUTABLE OFF
)

if(WIN32)
    target_link_libraries(Injector PRIVATE
        kernel32
    )
endif()
