cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(optional_view)


add_library(${PROJECT_NAME} INTERFACE)


target_include_directories(${PROJECT_NAME} INTERFACE include)
