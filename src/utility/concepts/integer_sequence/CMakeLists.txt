target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        integer_sequence.cppm
        index_sequence_c.ixx
        integer_sequence_c.ixx
)
