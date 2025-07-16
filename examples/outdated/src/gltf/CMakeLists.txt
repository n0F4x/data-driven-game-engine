project(gltf)

add_executable(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PRIVATE
        VULKAN_HPP_NO_TO_STRING
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
)

target_sources(${PROJECT_NAME} PRIVATE
        init.cxx
        DemoApp.cxx
        main.cpp
)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
        demo.ixx
        init.ixx
        DemoApp.ixx
)

target_link_libraries(${PROJECT_NAME} PRIVATE base)
