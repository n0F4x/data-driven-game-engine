target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        integer_sequence.cppm
        integer_sequence_concat.ixx
        index_sequence_filter.ixx
        integer_sequence_offset.ixx
        integer_sequence_size.ixx
        integer_sequence_sort.ixx
        integer_sequence_to.ixx
        is_index_sequence.ixx
        is_integer_sequence.ixx
)
