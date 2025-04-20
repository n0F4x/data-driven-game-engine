add_executable(snake)

target_compile_features(snake PRIVATE cxx_std_26)

target_compile_options(snake PRIVATE
        -Wall
        -Wextra
        -pedantic-errors
        -Wconversion
        -Werror
        -Wno-missing-field-initializers
)

target_sources(snake PRIVATE
        FILE_SET CXX_MODULES
        BASE_DIRS ${CURRENT_SOURCE_DIR}
        FILES
        Window.ixx
)

target_sources(snake PRIVATE
        main.cpp
)

target_link_libraries(snake PRIVATE ${PROJECT_NAME})
