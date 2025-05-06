add_executable(simple)

target_compile_features(simple PRIVATE cxx_std_26)

target_compile_options(simple PRIVATE
        -Wall
        -Wextra
        -pedantic-errors
        -Wconversion
        -Werror
        -Wno-missing-field-initializers
)

target_sources(simple PRIVATE
        FILE_SET CXX_MODULES
        BASE_DIRS ${CURRENT_SOURCE_DIR}
        FILES
        Window.ixx
)

target_sources(simple PRIVATE
        main.cpp
)

target_link_libraries(simple PRIVATE ${PROJECT_NAME})
