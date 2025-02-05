target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        integer_sequence.cppm
        concat_integer_sequence.ixx
        integer_sequence_size.ixx
        is_index_sequence.ixx
        is_integer_sequence.ixx
        offset_integer_sequence.ixx
)
