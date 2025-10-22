target_sources(${PROJECT_NAME} PRIVATE
        initialize.cxx
        update.cxx
)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        tasks.cppm
        initialize.ixx
        update.ixx
)
