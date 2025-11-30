find_program(GLSLC glslc)
if (NOT GLSLC STREQUAL GLSLC-NOTFOUND)
    set(SHADERS_IN_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    set(SHADERS_OUT_DIR ${SHADERS_IN_DIR})

    list(APPEND SHADERS
            "${SHADERS_IN_DIR}/main.vert"
            "${SHADERS_IN_DIR}/simple_image.frag"
            "${SHADERS_IN_DIR}/virtual_image_with_request.frag"
            "${SHADERS_IN_DIR}/virtual_image_without_request.frag"
    )

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

    add_custom_target(${PROJECT_NAME}_build_shaders DEPENDS ${SHADER_OUT_NAMES})

    add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_build_shaders)
endif ()
