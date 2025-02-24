add_subdirectory(concepts)
add_subdirectory(reflection)
add_subdirectory(type_traits)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        meta.cppm
        int_at_least.ixx
        uint_at_least.ixx
)
