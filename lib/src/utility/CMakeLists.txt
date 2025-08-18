add_subdirectory(containers)
add_subdirectory(memory)
add_subdirectory(meta)
add_subdirectory(tuple)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        utility.cppm
        all_same.ixx
        contracts.ixx
        hashing.ixx
        no_op.ixx
        not_fn.ixx
        Overloaded.ixx
        ScopeGuard.ixx
        Size.ixx
        Strong.ixx
        TypeList.ixx
        ValueSequence.ixx
        wrap_ref.ixx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_headers
        TYPE HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        lifetime_bound.hpp
        contracts_macros.hpp
)
