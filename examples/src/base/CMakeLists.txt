project(base)

add_library(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PRIVATE
        VULKAN_HPP_NO_TO_STRING
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
)

target_sources(${PROJECT_NAME} PRIVATE
        DemoBase.cxx
        Controller.cxx
        init.cxx
        Renderer.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        Controller.ixx
        DemoBase.ixx
        init.ixx
        Renderer.ixx
)

target_link_libraries(${PROJECT_NAME} PUBLIC engine_lib)
