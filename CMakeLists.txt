cmake_minimum_required(VERSION 3.25)

project(engine C CXX)


add_library(${PROJECT_NAME})


option(engine_debug "Turn on debug mode for library" OFF)


# we only support Clang and/or GCC as of now
target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -pedantic-errors
        -Wconversion
        -Werror
        -Wno-missing-field-initializers
        $<$<COMPILE_LANGUAGE:CXX>:-std=c++2c>
)


include(dependencies.cmake)


target_include_directories(${PROJECT_NAME} PUBLIC src)

add_subdirectory(src)


###########
## TESTS ##
###########
add_subdirectory(example)


unset(engine_debug CACHE)
