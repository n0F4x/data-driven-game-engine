add_executable(test
        src/main.cpp
        src/Controller.cpp
        src/demo.cpp
        src/DemoRenderer.cpp
        src/demo_init.cpp
)
# TODO: use cxx_std_26
target_compile_features(test PRIVATE cxx_std_23)
target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -pedantic-errors
        -Wconversion
        -Werror
        -Wno-missing-field-initializers
        $<$<COMPILE_LANGUAGE:CXX>:-std=c++2c>
)
target_compile_definitions(test PRIVATE
        VULKAN_HPP_NO_TO_STRING
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
)
if (engine_debug)
    target_compile_definitions(test PRIVATE
            SPDLOG_ACTIVE_LEVEL=${spdlog_level}
    )
endif ()
target_link_libraries(test PRIVATE ${PROJECT_NAME})
