target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        addons.cppm
        ecs.ixx
        EventManager.ixx
        ResourceManager.ixx
)
