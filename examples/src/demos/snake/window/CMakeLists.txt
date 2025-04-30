target_sources(snake PRIVATE
        FILE_SET CXX_MODULES
        BASE_DIRS ${SNAKE_SOURCE_DIR}
        FILES
        window.cppm
        events.ixx
        Plugin.ixx
        Setting.ixx
        Window.ixx
)
