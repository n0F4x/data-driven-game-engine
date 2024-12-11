add_subdirectory(renderer)
add_subdirectory(window)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        plugins.cppm
)
