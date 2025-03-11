target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        integer_sequence.cppm
        index_sequence.ixx
        integer_sequence.ixx
)
