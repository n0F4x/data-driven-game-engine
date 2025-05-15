add_subdirectory(containers)
add_subdirectory(memory)
add_subdirectory(meta)
add_subdirectory(tuple)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        utility.cppm
        all_same.ixx
        contracts.ixx
        hashing.ixx
        overloaded.ixx
        ScopeGuard.ixx
        Size.ixx
        Strong.ixx
        TypeList.ixx
        ValueSequence.ixx
        wrap_ref.ixx
)

if (${project_prefix}ENABLE_TESTS)
    target_sources(${PROJECT_NAME} PUBLIC
            FILE_SET engine_inteface_headers
            TYPE HEADERS
            BASE_DIRS ${PROJECT_SOURCE_DIR}/src
            FILES
            contracts_macros.hpp
    )
else ()
    target_sources(${PROJECT_NAME} PRIVATE
            FILE_SET engine_headers
            TYPE HEADERS
            BASE_DIRS ${PROJECT_SOURCE_DIR}/src
            FILES
            contracts_macros.hpp
    )
endif ()

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET engine_headers
        TYPE HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        lifetime_bound.hpp
)
