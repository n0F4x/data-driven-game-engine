project(tests)

add_executable(${PROJECT_NAME})

target_compile_options(${PROJECT_NAME} PRIVATE
        -ftemplate-backtrace-limit=0
)

target_link_libraries(${PROJECT_NAME} PRIVATE engine)

find_package(Catch2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})

add_subdirectory(src)
