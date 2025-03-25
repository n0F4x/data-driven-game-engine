add_subdirectory(query)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        ecs.cppm
        Archetype.ixx
        ArchetypeContainer.ixx
        ArchetypeID.ixx
        ArchetypeInfo.ixx
        component_c.ixx
        ComponentContainer.ixx
        ComponentID.ixx
        ComponentTable.ixx
        ComponentTag.ixx
        ErasedComponentContainer.ixx
        ID.ixx
        RecordID.ixx
        RecordIndex.ixx
        Registry.ixx
        sorted_component_id_sequence.ixx
)
