target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        ecs.cppm
        DependencyProvider.ixx
        accessors.ixx
)
