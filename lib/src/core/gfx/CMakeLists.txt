add_subdirectory(resources)

target_sources(${PROJECT_NAME} PRIVATE
        Camera.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        gfx.cppm
        Camera.ixx
)
