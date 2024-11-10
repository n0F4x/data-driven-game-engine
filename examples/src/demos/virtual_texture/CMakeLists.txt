add_executable(virtual_texture)
# TODO: use cxx_std_26
target_compile_features(virtual_texture PRIVATE cxx_std_23)
set_target_properties(virtual_texture PROPERTIES CXX_STANDARD 26)
target_compile_options(virtual_texture PRIVATE
        -Wall
        -Wextra
        -pedantic-errors
        -Wconversion
        -Werror
        -Wno-missing-field-initializers
)
target_compile_definitions(virtual_texture PRIVATE
        VULKAN_HPP_NO_TO_STRING
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
)
if (engine_debug)
    target_compile_definitions(virtual_texture PRIVATE
            SPDLOG_ACTIVE_LEVEL=${spdlog_level}
    )
endif ()
target_sources(virtual_texture PRIVATE
        demo.cpp
        DemoApp.cpp
        init.cpp
        main.cpp
        VirtualTexture.cpp
)
target_link_libraries(virtual_texture PRIVATE base)
target_link_libraries(virtual_texture PRIVATE ${PROJECT_NAME})

add_subdirectory(shaders)
