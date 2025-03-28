add_subdirectory(containers)
add_subdirectory(memory)
add_subdirectory(meta)
add_subdirectory(tuple)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        utility.cppm
        all_same.ixx
        hashing.ixx
        overloaded.ixx
        ScopeGuard.ixx
        Size.ixx
        Strong.ixx
        TypeList.ixx
        ValueSequence.ixx
)

target_sources(${PROJECT_NAME} PRIVATE
        contracts.cpp
)

if (engine_test)
    target_sources(${PROJECT_NAME} PUBLIC
            FILE_SET engine_inteface_headers
            TYPE HEADERS
            BASE_DIRS ${ENGINE_SRC_DIR}
            FILES
            contracts.hpp
    )
else ()
    target_sources(${PROJECT_NAME} PRIVATE
            FILE_SET engine_headers
            TYPE HEADERS
            BASE_DIRS ${ENGINE_SRC_DIR}
            FILES
            contracts.hpp
    )
endif ()

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET engine_headers
        TYPE HEADERS
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        lifetime_bound.hpp
)
