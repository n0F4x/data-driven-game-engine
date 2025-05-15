add_subdirectory(query)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        ecs.cppm
        ArchetypeID.ixx
        Archetype.ixx
        component_c.ixx
        ComponentContainer.ixx
        ComponentID.ixx
        ComponentTable.extensions.ixx
        ComponentTable.ixx
        ComponentTableMap.extensions.ixx
        ComponentTableMap.ixx
        ComponentTag.ixx
        decays_to_component_c.ixx
        ErasedComponentTable.extensions.ixx
        ErasedComponentTable.ixx
        ID.ixx
        LookupTable.ixx
        LookupTableMap.ixx
        RecordID.ixx
        RecordIndex.ixx
        Registry.ixx
)
