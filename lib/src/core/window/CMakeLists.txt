target_sources(${PROJECT_NAME} PRIVATE
        events.cxx
        Window.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        window.cppm
        CursorMode.ixx
        events.ixx
        Key.ixx
        Window.ixx
)
