target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        addons.cppm
        ecs.ixx
        EventManager.ixx
        ResourceManager.ixx
)
