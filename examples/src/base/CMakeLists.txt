add_library(base)
# TODO: use cxx_std_26
target_compile_features(base PRIVATE cxx_std_23)
set_target_properties(base PROPERTIES CXX_STANDARD 26)
target_compile_options(base PRIVATE
        -Wall
        -Wextra
        -pedantic-errors
        -Wconversion
        -Werror
        -Wno-missing-field-initializers
)
target_compile_definitions(base PRIVATE
        VULKAN_HPP_NO_TO_STRING
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
)
if (engine_debug)
    target_compile_definitions(base PRIVATE
            SPDLOG_ACTIVE_LEVEL=${spdlog_level}
    )
endif ()
target_include_directories(base PUBLIC ../../src)
target_sources(base PRIVATE
        DemoBase.cpp
        Controller.cpp
        init.cpp
        Renderer.cpp
)
target_link_libraries(base PRIVATE ${PROJECT_NAME})
