add_subdirectory(tasks)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        window.cppm
        display_refresh_duration.ixx
        DisplayTimer.ixx
        Settings.ixx
        setup.ixx
        Window.ixx
)
