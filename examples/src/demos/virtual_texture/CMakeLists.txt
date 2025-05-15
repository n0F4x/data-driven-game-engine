project(virtual_texture)

add_executable(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PRIVATE
        VULKAN_HPP_NO_TO_STRING
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
)

target_sources(${PROJECT_NAME} PRIVATE
        DemoApp.cxx
        init.cxx
        main.cpp
        VirtualTexture.cxx
)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
        Camera.ixx
        demo.ixx
        DemoApp.ixx
        init.ixx
        Vertex.ixx
        VirtualTexture.ixx
        VirtualTextureInfo.ixx
)

target_link_libraries(${PROJECT_NAME} PRIVATE base)

add_subdirectory(shaders)
