add_subdirectory(app)
add_subdirectory(core)
add_subdirectory(plugins)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        app.ixx
        plugins.ixx
)
