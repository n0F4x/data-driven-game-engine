add_subdirectory(query)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        ecs.cppm
        ArchetypeID.ixx
        Archetype.ixx
        component_c.ixx
        ComponentContainer.ixx
        ComponentID.ixx
        ComponentTable.ixx
        ComponentTableMap.ixx
        ComponentTag.ixx
        ErasedComponentContainer.ixx
        ErasedComponentContainer-fwd.ixx
        ID.ixx
        LookupTable.ixx
        LookupTableMap.ixx
        RecordID.ixx
        RecordIndex.ixx
        Registry.ixx
)
