add_subdirectory(functional)
add_subdirectory(integer_sequence)
add_subdirectory(type_list)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        type_traits.cppm
        add_wrapper.ixx
        all_same.ixx
        back.ixx
        const_like.ixx
        forward_like.ixx
        is_specialization_of.ixx
        maybe_const.ixx
        remove_wrapper_if.ixx
        tuple_or_pair.ixx
        underlying.ixx
)
