add_subdirectory(events)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        accessors.cppm
        ecs.ixx
        resource.ixx
)
