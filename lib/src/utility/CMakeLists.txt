add_subdirectory(containers)
add_subdirectory(memory)
add_subdirectory(meta)
add_subdirectory(tuple)

target_sources(${PROJECT_NAME} PRIVATE
        contract.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        utility.cppm
        all_same.ixx
        any_cast.ixx
        contracts.ixx
        FunctionInterface.ixx
        hashing.ixx
        no_op.ixx
        not_fn.ixx
        Overloaded.ixx
        ScopeFail.ixx
        Size.ixx
        StringLiteral.ixx
        TypeList.ixx
        ValueSequence.ixx
        wrap_ref.ixx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_headers
        TYPE HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        contract_macros.hpp
        lifetimebound.hpp
        no_unique_address.hpp
)
