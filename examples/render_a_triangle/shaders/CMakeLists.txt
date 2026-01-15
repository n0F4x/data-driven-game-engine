find_program(SLANGC slangc HINTS $ENV{VULKAN_SDK}/bin REQUIRED)

set(SHADERS_IN_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SHADERS_OUT_DIR ${SHADERS_IN_DIR})

file(GLOB_RECURSE SHADERS "${SHADERS_IN_DIR}/*.slang")

file(MAKE_DIRECTORY ${SHADERS_OUT_DIR})

foreach (SHADER ${SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
    set(SHADER_OUT_FILE "${SHADERS_OUT_DIR}/${SHADER_NAME}.spv")
    list(APPEND SHADER_OUT_FILES ${SHADER_OUT_FILE})
    add_custom_command(
            MAIN_DEPENDENCY ${SHADER}
            OUTPUT ${SHADER_OUT_FILE}
            COMMAND ${SLANGC} ${SHADER} -target spirv -profile spirv_1_4 -emit-spirv-directly -o ${SHADER_OUT_FILE}
            VERBATIM
    )
endforeach ()

add_custom_target(${PROJECT_NAME}_shaders DEPENDS ${SHADER_OUT_FILES})

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_shaders)
