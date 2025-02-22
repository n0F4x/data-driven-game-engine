add_subdirectory(sparse_set)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        containers.cppm
        MultiSparseSet.ixx
        SparseSet.ixx
)
