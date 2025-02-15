target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ../../../../..
        FILES
        type_list.cppm
        all_of_type_list_c.ixx
        type_list_c.ixx
)
