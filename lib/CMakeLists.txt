project(engine_lib)

set(${project_prefix}LOG_LEVEL ${project_prefix}LOG_LEVEL_INFO CACHE STRING "Set log level")
set_property(CACHE ${project_prefix}LOG_LEVEL PROPERTY STRINGS
        ${project_prefix}LOG_LEVEL_OFF
        ${project_prefix}LOG_LEVEL_CRITICAL
        ${project_prefix}LOG_LEVEL_ERROR
        ${project_prefix}LOG_LEVEL_WARNING
        ${project_prefix}LOG_LEVEL_INFO
        ${project_prefix}LOG_LEVEL_DEBUG
        ${project_prefix}LOG_LEVEL_TRACE
)


add_library(${PROJECT_NAME})
add_library(engine::engine ALIAS ${PROJECT_NAME})


target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_26)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        CHECK_LIFETIME_BOUND
        ENGINE_LOG_LEVEL=${project_prefix}LOG_LEVEL
)

if (${project_prefix}ENABLE_TESTS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            ${project_prefix}ENABLE_UNIT_TESTS
    )
endif ()


include(cmake/dependencies.cmake)


add_subdirectory(src)
