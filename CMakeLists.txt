cmake_minimum_required(VERSION 3.30)

project(engine)

set(project_prefix ENGINE_)

option(${project_prefix}USE_ASAN "Use AddressSanitizer" OFF)
option(${project_prefix}USE_UBSAN "Use UndefinedBehaviorSanitizer" OFF)


# We only support Clang for now
add_compile_options(
        -Werror
        -Wall
        -Wextra
        -pedantic-errors
        -Wconversion
        -Wno-missing-field-initializers
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wcast-qual
)

if (${project_prefix}USE_ASAN)
    add_compile_options(
            -fsanitize=address
            -fno-omit-frame-pointer
    )
    add_link_options(-fsanitize=address)
endif ()

if (${project_prefix}USE_UBSAN)
    add_compile_options(
            -fsanitize=undefined
    )
    add_link_options(-fsanitize=undefined)
endif ()


add_subdirectory(lib)

if (${project_prefix}ENABLE_TESTS)
    add_subdirectory(tests)
endif ()

if (${project_prefix}ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif ()
