add_subdirectory(modules)
add_subdirectory(util)

target_sources(${PROJECT_NAME} PRIVATE
        test_test.cpp
)
