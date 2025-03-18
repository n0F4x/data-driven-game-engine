target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        tuple.cppm
        generate_tuple_from.ixx
        tuple_all_of.ixx
        tuple_any_of.ixx
        tuple_drop_back.ixx
        tuple_drop_front.ixx
        tuple_drop_nth.ixx
        tuple_select.ixx
)
