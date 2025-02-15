add_subdirectory(meta)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        utility.cppm
        hashing.ixx
        OptionalRef.ixx
        ScopeGuard.ixx
        Size.ixx
        SparseSet.ixx
        tuple.ixx
)
