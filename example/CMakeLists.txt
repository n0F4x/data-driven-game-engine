add_executable(test)
# TODO: use cxx_std_26
target_compile_features(test PRIVATE cxx_std_23)
set_target_properties(test PROPERTIES CXX_STANDARD 26)
target_compile_options(test PRIVATE
        -Wall
        -Wextra
        -pedantic-errors
        -Wconversion
        -Werror
        -Wno-missing-field-initializers
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
add_subdirectory(src)
target_link_libraries(test PRIVATE ${PROJECT_NAME})
