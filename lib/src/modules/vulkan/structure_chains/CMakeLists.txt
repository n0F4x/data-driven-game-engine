target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        structure_chains.cppm
        extends_struct_c.ixx
        feature_struct_c.ixx
        filter_physical_device_features.ixx
        match_physical_device_features.ixx
        merge_physical_device_features.ixx
        promoted_feature_struct_c.ixx
        remove_physical_device_features.ixx
        StructureChain.ixx
)
