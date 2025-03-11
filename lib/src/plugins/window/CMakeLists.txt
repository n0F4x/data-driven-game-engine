target_sources(${PROJECT_NAME} PRIVATE
        WindowPlugin.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        window.cppm
        WindowPlugin.ixx
)
