target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        tuple.cppm
        generate_tuple_from.ixx
        tuple_all_of.ixx
        tuple_any_of.ixx
        tuple_drop_back.ixx
        tuple_drop_front.ixx
        tuple_drop_nth.ixx
        tuple_for_each.ixx
        tuple_select.ixx
        tuple_transform.ixx
)
