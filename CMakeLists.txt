cmake_minimum_required(VERSION 3.30)

project(engine C CXX)


add_library(${PROJECT_NAME})


option(engine_debug "Turn on debug mode for library" OFF)
option(engine_test "Turn on debug mode for library" OFF)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_26)

# we only support Clang and/or GCC as of now
target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -pedantic-errors
        -Wconversion
        -Werror
        -Wno-missing-field-initializers
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    CHECK_LIFETIME_BOUND
)
if (engine_test)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            ENGINE_ENABLE_TESTS
    )
endif ()


include(cmake/dependencies.cmake)


target_include_directories(${PROJECT_NAME} PUBLIC src)

add_subdirectory(src)


add_subdirectory(examples)


unset(engine_debug CACHE)
