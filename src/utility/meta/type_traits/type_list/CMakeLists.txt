target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        type_list.cppm
        is_type_list.ixx
        type_list_at.ixx
        type_list_chunk_sub.ixx
        type_list_concat.ixx
        type_list_contains.ixx
        type_list_disjoin.ixx
        type_list_drop.ixx
        type_list_drop_back.ixx
        type_list_drop_front.ixx
        type_list_is_sorted.ixx
        type_list_filter.ixx
        type_list_front.ixx
        type_list_join.ixx
        type_list_push_back.ixx
        type_list_push_front.ixx
        type_list_replace.ixx
        type_list_size.ixx
        type_list_sort.ixx
        type_list_take.ixx
        type_list_to.ixx
        type_list_transform.ixx
        type_list_unique.ixx
)
