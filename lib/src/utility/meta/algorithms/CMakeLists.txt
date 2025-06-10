target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        algorithms.cppm
        all_of.ixx
        any_of.ixx
        apply.ixx
        enumerate.ixx
        fold_left_first.ixx
        for_each.ixx
        none_of.ixx
)
