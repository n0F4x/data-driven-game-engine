cmake_minimum_required(VERSION 3.30)

project(engine-master)

set(project_prefix ENGINE_)

option(${project_prefix}ENABLE_TESTS "Enables building tests" OFF)
option(${project_prefix}ENABLE_EXAMPLES "Enables building examples" OFF)


# We only support Clang for now
add_compile_options(
        -Werror
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wno-missing-field-initializers
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wcast-qual
        -Wnrvo

        # TODO: remove this when there is no '-c' unused warning while testing package
        -Wno-unused-command-line-argument
)


add_subdirectory(lib)

if (${project_prefix}ENABLE_TESTS)
    add_subdirectory(tests)
endif ()

if (${project_prefix}ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif ()
