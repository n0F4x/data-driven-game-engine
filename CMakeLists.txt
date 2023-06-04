cmake_minimum_required(VERSION 3.21)
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(engine)


add_library(${PROJECT_NAME})


if (MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC
            /W4
            /WX
            )
else ()
    target_compile_options(${PROJECT_NAME} PUBLIC
            -Wall
            -Wextra
            -Werror
            -fconcepts-diagnostics-depth=3
            )
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC
        src
        )

add_subdirectory(src/app)
add_subdirectory(src/renderer)
add_subdirectory(src/schedule)
add_subdirectory(src/state_machine)
add_subdirectory(src/state)
add_subdirectory(src/window)


find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)

# SFML
set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)


target_link_libraries(${PROJECT_NAME} PUBLIC
        Microsoft.GSL::GSL
        Vulkan::Vulkan
        glm::glm
        glfw
        EnTT::EnTT
        sfml-window
        )


add_executable(test examples/src/main.cpp)
target_link_libraries(test ${PROJECT_NAME})
