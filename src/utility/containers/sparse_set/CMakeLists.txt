target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        key_c.ixx
        MultiSparseSet.ixx
        SparseSet.ixx
        SparseSetBase.ixx
        value_c.ixx
)
