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
        ScopeGuard.ixx
        Size.ixx
        Strong.ixx
        TypeList.ixx
        ValueSequence.ixx
)
