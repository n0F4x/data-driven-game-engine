add_subdirectory(resources)

target_sources(${PROJECT_NAME} PRIVATE
        Camera.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        gfx.cppm
        Camera.ixx
)
