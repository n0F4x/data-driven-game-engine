target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        algorithms.cppm
        any_of.ixx
        apply.ixx
        enumerate.ixx
        fold_left_first.ixx
        for_each.ixx
)
