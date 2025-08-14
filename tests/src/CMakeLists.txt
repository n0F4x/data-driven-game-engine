add_subdirectory(modules)
add_subdirectory(utility)

target_sources(${PROJECT_NAME} PRIVATE
        test_test.cpp
)
