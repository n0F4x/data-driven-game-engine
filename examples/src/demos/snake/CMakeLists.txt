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

target_link_libraries(snake PRIVATE ${PROJECT_NAME})

include(FetchContent)
fetchcontent_declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.1
        GIT_SHALLOW ON
        SYSTEM
)

fetchcontent_makeavailable(SFML)
target_link_libraries(snake PRIVATE SFML::Graphics)

add_subdirectory(window)

target_sources(snake PRIVATE
        main.cpp
)
