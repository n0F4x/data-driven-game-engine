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
        ComponentTable.extensions.ixx
        ComponentTable.ixx
        ComponentTable-fwd.ixx
        ComponentTableMap.extensions.ixx
        ComponentTableMap.ixx
        ComponentTag.ixx
        decays_to_component_c.ixx
        ErasedComponentContainer.ixx
        ID.ixx
        LookupTable.ixx
        LookupTableMap.ixx
        RecordID.ixx
        RecordIndex.ixx
        Registry.ixx
)
