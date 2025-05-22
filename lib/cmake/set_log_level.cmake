macro(set_log_level target level)
    if (${level} STREQUAL ${project_prefix}LOG_LEVEL_OFF)
        target_compile_definitions(${target} PUBLIC
                ENGINE_LOG_LEVEL=0
        )
    elseif (${level} STREQUAL ${project_prefix}LOG_LEVEL_CRITICAL)
        target_compile_definitions(${target} PUBLIC
                ENGINE_LOG_LEVEL=1
        )
    elseif (${level} STREQUAL ${project_prefix}LOG_LEVEL_ERROR)
        target_compile_definitions(${target} PUBLIC
                ENGINE_LOG_LEVEL=2
        )
    elseif (${level} STREQUAL ${project_prefix}LOG_LEVEL_WARNING)
        target_compile_definitions(${target} PUBLIC
                ENGINE_LOG_LEVEL=3
        )
    elseif (${level} STREQUAL ${project_prefix}LOG_LEVEL_INFO)
        target_compile_definitions(${target} PUBLIC
                ENGINE_LOG_LEVEL=4
        )
    elseif (${level} STREQUAL ${project_prefix}LOG_LEVEL_DEBUG)
        target_compile_definitions(${target} PUBLIC
                ENGINE_LOG_LEVEL=5
        )
    elseif (${level} STREQUAL ${project_prefix}LOG_LEVEL_TRACE)
        target_compile_definitions(${target} PUBLIC
                ENGINE_LOG_LEVEL=6
        )
    endif ()
endmacro()
