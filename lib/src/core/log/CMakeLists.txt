target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET engine_headers
        TYPE HEADERS
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        log.hpp
)

target_sources(${PROJECT_NAME}
        PRIVATE
        log.cpp
)
