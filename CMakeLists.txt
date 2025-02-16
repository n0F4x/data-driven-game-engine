cmake_minimum_required(VERSION 3.30)

project(engine C CXX)


if (NOT engine_test)
    add_library(${PROJECT_NAME})
else ()
    add_executable(${PROJECT_NAME})
endif ()


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

    find_package(Catch2 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain)
endif ()


include(cmake/dependencies.cmake)


target_include_directories(${PROJECT_NAME} PUBLIC src)

add_subdirectory(src)


if (NOT engine_test)
    add_subdirectory(examples)
else ()
    add_subdirectory(test)
endif ()


unset(engine_debug CACHE)
