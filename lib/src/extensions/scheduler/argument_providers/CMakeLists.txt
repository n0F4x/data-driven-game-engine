add_subdirectory(ecs)
add_subdirectory(events)
add_subdirectory(resources)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        argument_providers.cppm
)
