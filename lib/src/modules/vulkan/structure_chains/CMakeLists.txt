target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        structure_chains.cppm
        comprehensive_feature_struct_c.ixx
        core_feature_struct_from_vulkan1x_c.ixx
        erase_physical_device_features.ixx
        extends_struct_c.ixx
        feature_struct_c.ixx
        filter_physical_device_features.ixx
        individual_feature_struct_c.ixx
        is_empty_feature_sruct.ixx
        match_physical_device_features.ixx
        merge_physical_device_features.ixx
        part_of_comprehensive_feature_struct_c.ixx
        StructureChain.ixx
        vulkan1x_feature_struct_c.ixx
)
