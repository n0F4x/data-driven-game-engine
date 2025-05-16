target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_headers
        TYPE HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        log.hpp
)

target_sources(${PROJECT_NAME}
        PRIVATE
        log.cpp
)
