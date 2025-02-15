add_subdirectory(functional)
add_subdirectory(integer_sequence)
add_subdirectory(type_list)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        type_traits.cppm
        add_wrapper.ixx
        forward_like.ixx
        is_specialization_of.ixx
        remove_wrapper_if.ixx
        underlying.ixx
)
