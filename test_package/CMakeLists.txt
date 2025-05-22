cmake_minimum_required(VERSION 3.30.5)
project(test_package)

find_package(engine CONFIG REQUIRED)

add_executable(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE engine::engine)
target_sources(${PROJECT_NAME} PRIVATE src/main.cpp)
