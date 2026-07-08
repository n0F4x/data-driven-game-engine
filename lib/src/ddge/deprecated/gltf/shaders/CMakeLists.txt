find_program(GLSLC glslc)
if (NOT GLSLC STREQUAL GLSLC-NOTFOUND)
    set(SHADERS_IN_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
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
                COMMAND ${GLSLC} ${SHADER} "-o" ${SHADER_OUT_NAME}
                VERBATIM
        )
    endforeach ()

    add_custom_target(build_shaders DEPENDS ${SHADER_OUT_NAMES})

    add_dependencies(${PROJECT_NAME} build_shaders)
endif ()
