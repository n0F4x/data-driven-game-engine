target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        integer_sequence.cppm
        index_sequence_filter.ixx
        integer_sequence_concat.ixx
        integer_sequence_at.ixx
        integer_sequence_offset.ixx
        integer_sequence_size.ixx
        integer_sequence_sort.ixx
        integer_sequence_to.ixx
        integer_sequence_unique.ixx
        is_index_sequence.ixx
        is_integer_sequence.ixx
)
