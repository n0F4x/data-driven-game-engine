cmake_minimum_required(VERSION 3.21)
include(FetchContent)

set(CMAKE_CXX_STANDARD 23)

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
#            -Werror
            -fconcepts-diagnostics-depth=3
            )
endif ()


target_compile_definitions(${PROJECT_NAME} PUBLIC ENGINE_DEBUG)
get_target_property(engine_compile_defs ${PROJECT_NAME} COMPILE_DEFINITIONS)


target_include_directories(${PROJECT_NAME} PUBLIC
        src
        )

add_subdirectory(src/engine)


#find_package(Microsoft.GSL CONFIG REQUIRED)
#find_package(glm CONFIG REQUIRED)
#find_package(glfw3 CONFIG REQUIRED)
#find_package(EnTT CONFIG REQUIRED)

# Vulkan
find_package(Vulkan REQUIRED)
target_compile_definitions(${PROJECT_NAME} PUBLIC
        VULKAN_HPP_NO_EXCEPTIONS
        VULKAN_HPP_NO_TO_STRING
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SMART_HANDLE
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
        )
if (ENGINE_DEBUG IN_LIST engine_compile_defs)
    target_compile_definitions(${PROJECT_NAME} PUBLIC ENGINE_VULKAN_DEBUG)
endif ()
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)

# SFML
set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)
target_link_libraries(${PROJECT_NAME} PUBLIC
        sfml-window)


###########
## TESTS ##
###########
add_executable(test examples/src/main.cpp)
target_link_libraries(test PUBLIC ${PROJECT_NAME})
