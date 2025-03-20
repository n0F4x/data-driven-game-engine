target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        ecs.cppm
        Archetype.ixx
        ArchetypeContainer.ixx
        ArchetypeID.ixx
        component_c.ixx
        ComponentContainer.ixx
        ComponentID.ixx
        ComponentTable.ixx
        ComponentTag.ixx
        ErasedComponentContainer.ixx
        ID.ixx
        Query.ixx
        Query-fwd.ixx
        queryable_component_c.ixx
        RecordID.ixx
        RecordIndex.ixx
        Registry-fwd.ixx
        Registry.ixx
        RegistryTag.ixx
        sorted_component_id_sequence.ixx
        specialization_of_registry_c.ixx
)
