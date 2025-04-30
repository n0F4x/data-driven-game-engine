add_executable(snake)

set(SNAKE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(snake PRIVATE cxx_std_26)

target_compile_options(snake PRIVATE
        -Wall
        -Wextra
        -pedantic-errors
        -Wconversion
        -Werror
        -Wno-missing-field-initializers
)

add_subdirectory(window)

target_sources(snake PRIVATE
        main.cpp
)

target_link_libraries(snake PRIVATE ${PROJECT_NAME})
