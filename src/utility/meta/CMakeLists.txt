target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        meta.cppm
        functional.ixx
        tuple.ixx
        tuple_like.ixx
)
