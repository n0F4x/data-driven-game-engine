add_executable(test
        src/main.cpp
        src/Camera.cpp
        src/Controller.cpp
        src/demo.cpp
        src/demo_init.cpp
)
target_compile_features(test PRIVATE cxx_std_23)
target_compile_definitions(test PRIVATE
        VULKAN_HPP_NO_EXCEPTIONS
        VULKAN_HPP_NO_TO_STRING
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SMART_HANDLE
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
)
target_link_libraries(test PRIVATE ${PROJECT_NAME})

###################
## Build shaders ##
###################

find_program(GLSL_VALIDATOR glslangValidator HINTS
        ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        /usr/bin
        /usr/local/bin
        ${VULKAN_SDK_PATH}/Bin
        ${VULKAN_SDK_PATH}/Bin32
        "$ENV{VULKAN_SDK}/Bin/"
        "$ENV{VULKAN_SDK}/Bin32/"
)

# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/examples/shaders/*.frag"
        "${PROJECT_SOURCE_DIR}/examples/shaders/*.vert"
)

foreach (GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_SOURCE_DIR}/examples/shaders/${FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach (GLSL)

add_custom_target(
        shaders
        DEPENDS ${SPIRV_BINARY_FILES}
)
