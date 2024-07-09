cmake_minimum_required(VERSION 3.25)

project(engine C CXX)


add_library(${PROJECT_NAME})


option(engine_debug "Turn on debug mode for library" OFF)


target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)


if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
            /W4
            /WX
    )
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall
            -Wextra
            -pedantic-errors
            -Wconversion
            -Werror
            -Wno-missing-field-initializers
    )
endif ()


include(dependencies.cmake)


target_include_directories(${PROJECT_NAME} PUBLIC src)

add_subdirectory(src)


###########
## TESTS ##
###########
add_subdirectory(example)


unset(engine_debug CACHE)
