add_subdirectory(containers)
add_subdirectory(memory)
add_subdirectory(meta)
add_subdirectory(tuple)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        utility.cppm
        all_same.ixx
        hashing.ixx
        ScopeGuard.ixx
        Size.ixx
        TypeList.ixx
)
