target_sources(${PROJECT_NAME} PRIVATE
        Instance.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        instance.cppm
        Instance.ixx
)
