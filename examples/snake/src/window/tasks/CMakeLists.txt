target_sources(${PROJECT_NAME} PRIVATE
        clear_window.cxx
        close_window.cxx
        display.cxx
        initialize.cxx
        update.cxx
        window_should_close.cxx
)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        tasks.cppm
        clear_window.ixx
        close_window.ixx
        display.ixx
        initialize.ixx
        update.ixx
        window_should_close.ixx
)
