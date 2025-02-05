add_subdirectory(concepts)
add_subdirectory(type_traits)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        utility.cppm
        hashing.ixx
        Size.ixx
        tuple.ixx
)
