target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        containers.cppm
        Any.ixx
        SlotMultiMap.ixx
        OptionalRef.ixx
        SlotMap.ixx
        SparseSet.ixx
        StackedTuple.ixx
)
