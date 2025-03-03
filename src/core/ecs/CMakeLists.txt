target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        ecs.cppm
        Archetype.ixx
        Component.ixx
        Entity.ixx
        Registry.ixx
)
