add_subdirectory(functional)
add_subdirectory(tuple)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        meta.cppm
        concat.ixx
        offset.ixx
)
