target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        containers.cppm
        Any.ixx
        FunctionWrapper.ixx
        SlotMultiMap.ixx
        OptionalRef.ixx
        Ref.ixx
        SlotMap.ixx
        SparseSet.ixx
        StackedTuple.ixx
)
