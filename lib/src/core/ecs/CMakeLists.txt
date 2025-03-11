target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        ecs.cppm
        Archetype.ixx
        Component.ixx
        Entity.ixx
        fwd.ixx
        Registry.ixx
        RegistryTag.ixx
        specialization_of_registry_c.ixx
)
