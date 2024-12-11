add_subdirectory(meta)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ../..
        FILES
        utility.cppm
        hashing.ixx
        Size.ixx
        tuple.ixx
)
