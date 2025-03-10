target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        integer_sequence.cppm
        integer_sequence_concat.ixx
        integer_sequence_size.ixx
        integer_sequence_sort.ixx
        is_index_sequence.ixx
        is_integer_sequence.ixx
        integer_sequence_offset.ixx
)
