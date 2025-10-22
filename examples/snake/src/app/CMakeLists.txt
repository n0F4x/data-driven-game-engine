add_subdirectory(tasks)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        app.cppm
        create.ixx
        setup.ixx
        run.ixx
)
