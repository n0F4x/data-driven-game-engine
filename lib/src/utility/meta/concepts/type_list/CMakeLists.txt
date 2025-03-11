target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        type_list.cppm
        all_of_type_list.ixx
        type_list.ixx
)
