add_subdirectory(core)
add_subdirectory(utility)

target_sources(${PROJECT_NAME} PRIVATE
        test_test.cpp
)
