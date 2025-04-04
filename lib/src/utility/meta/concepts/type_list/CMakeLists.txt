target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        type_list.cppm
        type_list.ixx
        type_list_all_different.ixx
        type_list_all_of.ixx
        type_list_all_same.ixx
        type_list_none_of.ixx
)
