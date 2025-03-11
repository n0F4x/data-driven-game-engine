add_subdirectory(functional)
add_subdirectory(integer_sequence)
add_subdirectory(type_list)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        type_traits.cppm
        add_wrapper.ixx
        all_different.ixx
        all_same.ixx
        forward_like.ixx
        is_specialization_of.ixx
        remove_wrapper_if.ixx
        select_last.ixx
        underlying.ixx
)
