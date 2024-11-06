add_executable(gltf)
# TODO: use cxx_std_26
target_compile_features(gltf PRIVATE cxx_std_23)
set_target_properties(gltf PROPERTIES CXX_STANDARD 26)
target_compile_options(gltf PRIVATE
        -Wall
        -Wextra
        -pedantic-errors
        -Wconversion
        -Werror
        -Wno-missing-field-initializers
)
target_compile_definitions(gltf PRIVATE
        VULKAN_HPP_NO_TO_STRING
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
)
if (engine_debug)
    target_compile_definitions(gltf PRIVATE
            SPDLOG_ACTIVE_LEVEL=${spdlog_level}
    )
endif ()
target_sources(gltf PRIVATE
        demo.cpp
        init.cpp
        DemoApp.cpp
        main.cpp
)
target_link_libraries(gltf PRIVATE base)
target_link_libraries(gltf PRIVATE ${PROJECT_NAME})
