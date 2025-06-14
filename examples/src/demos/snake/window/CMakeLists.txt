target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        window.cppm
        clear_window.ixx
        close_window.ixx
        display.ixx
        display_rate.ixx
        initialize.ixx
        open_window.ixx
        Settings.ixx
        setup.ixx
        update.ixx
        Window.ixx
        window_should_close.ixx
)
