add_executable(test
        src/main.cpp
        src/Camera.cpp
        src/Controller.cpp
        src/demo.cpp
        src/DemoRenderer.cpp
        src/demo_init.cpp
)
target_compile_features(test PRIVATE cxx_std_23)
target_compile_definitions(test PRIVATE
        VULKAN_HPP_NO_TO_STRING
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
)
target_link_libraries(test PRIVATE ${PROJECT_NAME})

###################
## Build shaders ##
###################
set(SHADERS_IN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADERS_OUT_DIR ${SHADERS_IN_DIR})

file(GLOB_RECURSE SHADERS "${SHADERS_IN_DIR}/*.vert" "${SHADERS_IN_DIR}/*.frag")

file(MAKE_DIRECTORY ${SHADERS_OUT_DIR})

foreach (SHADER ${SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SHADER_OUT_NAME "${SHADERS_OUT_DIR}/${SHADER_NAME}.spv")
    list(APPEND SHADER_OUT_NAMES ${SHADER_OUT_NAME})
    add_custom_command(
            MAIN_DEPENDENCY ${SHADER}
            OUTPUT ${SHADER_OUT_NAME}
            COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} "-o" ${SHADER_OUT_NAME}
            VERBATIM)
endforeach ()

add_custom_target(build_shaders DEPENDS ${SHADER_OUT_NAMES})

add_dependencies(test build_shaders)
