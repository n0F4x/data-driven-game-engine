add_executable(constexpr)
target_compile_features(constexpr PRIVATE cxx_std_26)
target_compile_options(constexpr PRIVATE
        -Wall
        -Wextra
        -pedantic-errors
        -Wconversion
        -Werror
        -Wno-missing-field-initializers
        -v
)

target_sources(constexpr PRIVATE
        main.cpp
)

target_link_libraries(constexpr PRIVATE ${PROJECT_NAME})
