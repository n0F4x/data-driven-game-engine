target_sources(${PROJECT_NAME} PRIVATE
        events.cxx
        Window.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        window.cppm
        CursorMode.ixx
        events.ixx
        Key.ixx
        Window.ixx
)
