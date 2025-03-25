add_subdirectory(functional)
add_subdirectory(integer_sequence)
add_subdirectory(ranges)
add_subdirectory(type_list)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        concepts.cppm
        all_different.ixx
        allocator.ixx
        decayed.ixx
        decays_to.ixx
        nothrow_assignable_from.ixx
        nothrow_constructible_from.ixx
        nothrow_convertible_to.ixx
        nothrow_movable.ixx
        nothrow_move_constructible.ixx
        specialization_of.ixx
        tuple_like.ixx
)
