target_sources(${PROJECT_NAME} PRIVATE
        DependencyChainNode.cxx
        EntryBuilderContainer.cxx
        EntryInjectionContainer.cxx
        RegistryBuilder.cxx
        ReverseDependencyChainNode.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        registry.cppm
        BuildableEntry.ixx
        BuildableEntryBase.ixx
        BuildableEntryBuilder.ixx
        BuildableEntryBuilderBase.ixx
        BuildDirector.ixx
        configuration_entry_c.ixx
        ConfigurationEntry.ixx
        CyclicDependencyDetected.ixx
        DependencyChainNode.ixx
        entry_c.ixx
        entry_builder_c.ixx
        EntryBase.ixx
        EntryBuilderBase.ixx
        EntryBuilderContainer.ixx
        EntryBuilderFailedError.ixx
        EntryInjectionContainer.ixx
        ErasedEntry.ixx
        Registry.ixx
        RegistryBuilder.ixx
        represents_entry_builder_dependency_c.ixx
        represents_entry_dependency_c.ixx
        ReverseDependencyChainNode.ixx
        strip_dependency_t.ixx
)
