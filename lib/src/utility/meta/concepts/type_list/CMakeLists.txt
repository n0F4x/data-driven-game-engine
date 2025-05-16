target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        type_list.cppm
        type_list.ixx
        type_list_all_different.ixx
        type_list_all_of.ixx
        type_list_all_same.ixx
        type_list_none_of.ixx
)
