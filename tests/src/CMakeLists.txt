add_subdirectory(core)
add_subdirectory(utility)

target_sources(tests PRIVATE
        test_test.cpp
)
