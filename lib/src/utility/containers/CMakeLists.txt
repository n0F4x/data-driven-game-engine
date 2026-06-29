add_subdirectory(store)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        containers.cppm
        Any.ixx
        AnyCopyable.ixx
        AnyMoveOnly.ixx
        CopyableFunction.ixx
        Function.ixx
        FunctionWrapper.ixx
        MoveOnlyFunction.ixx
        SlotMultiMap.ixx
        OptionalRef.ixx
        Ref.ixx
        SlotMap.ixx
        SmallBuffer.ixx
        SparseSet.ixx
        StackedTuple.ixx
        Strong.ixx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_headers
        TYPE HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        Function.impl.hpp
)
