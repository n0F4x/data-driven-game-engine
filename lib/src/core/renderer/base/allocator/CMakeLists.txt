target_sources(${PROJECT_NAME} PRIVATE
        Allocator.cxx
        Requirements.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        allocator.cppm
        Allocator.ixx
)
