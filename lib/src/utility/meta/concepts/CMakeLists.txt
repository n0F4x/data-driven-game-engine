add_subdirectory(functional)
add_subdirectory(integer_sequence)
add_subdirectory(ranges)
add_subdirectory(type_list)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        concepts.cppm
        all_different.ixx
        allocator.ixx
        decayed.ixx
        decays_to.ixx
        generic_allocator.ixx
        hashable.ixx
        naked.ixx
        nothrow_assignable_from.ixx
        nothrow_constructible_from.ixx
        nothrow_convertible_to.ixx
        nothrow_movable.ixx
        nothrow_move_constructible.ixx
        storable.ixx
        strips_to.ixx
        specialization_of.ixx
        tuple_like.ixx
)
