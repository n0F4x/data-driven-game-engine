add_executable(tests)

target_compile_options(tests PRIVATE
        -Wall
        -Wextra
        -pedantic-errors
        -Wconversion
        -Werror
)

set(TESTS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(tests PRIVATE ${PROJECT_NAME})

find_package(Catch2 CONFIG REQUIRED)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

add_subdirectory(src)
