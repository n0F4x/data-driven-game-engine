add_executable(gltf)
target_compile_features(gltf PRIVATE cxx_std_26)
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
        init.cxx
        DemoApp.cxx
        main.cpp
)
target_sources(base PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
        demo.ixx
        init.ixx
        DemoApp.ixx
)
target_link_libraries(gltf PRIVATE base)
target_link_libraries(gltf PRIVATE ${PROJECT_NAME})
