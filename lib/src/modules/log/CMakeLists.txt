target_sources(${PROJECT_NAME}
        PRIVATE
        log.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_headers
        TYPE HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        log_level_definitions.hpp
        log_macros.hpp
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        log.cppm
        Level.ixx
        log.ixx
        log_level.ixx
)
