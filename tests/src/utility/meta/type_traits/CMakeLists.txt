add_subdirectory(functional)
add_subdirectory(integer_sequence)
add_subdirectory(type_list)

target_sources(tests PRIVATE
        as_value.cpp
        back.cpp
)
