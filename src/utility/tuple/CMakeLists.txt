target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        tuple.cppm
        generate_tuple_from.ixx
        tuple_drop_back.ixx
        tuple_drop_front.ixx
        tuple_drop_nth.ixx
        tuple_select.ixx
)
