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

## Microsoft.GSL
#FetchContent_Declare(Microsoft.GSL
#        GIT_REPOSITORY https://github.com/microsoft/GSL.git
#        GIT_TAG v4.0.0)
#FetchContent_MakeAvailable(Microsoft.GSL)
#target_link_libraries(${PROJECT_NAME} PUBLIC
#        Microsoft.GSL::GSL)

# SFML
set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)
target_link_libraries(${PROJECT_NAME} PUBLIC
        sfml-window)

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

# VulkanMemoryAllocator
FetchContent_Declare(VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        )
FetchContent_MakeAvailable(VulkanMemoryAllocator)
set(VMA_static_vulkan_functions 0)
target_compile_definitions(${PROJECT_NAME} PUBLIC
        VMA_STATIC_VULKAN_FUNCTIONS=${VMA_static_vulkan_functions}
        VMA_DYNAMIC_VULKAN_FUNCTIONS
        )
target_link_libraries(${PROJECT_NAME} PUBLIC
        VulkanMemoryAllocator)

# glm
FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8)
FetchContent_MakeAvailable(glm)
target_link_libraries(${PROJECT_NAME} PUBLIC
        glm::glm)

# EnTT
FetchContent_Declare(EnTT
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.12.2)
FetchContent_MakeAvailable(EnTT)
target_link_libraries(${PROJECT_NAME} PUBLIC
        EnTT)

## Taskflow
#FetchContent_Declare(Taskflow
#        GIT_REPOSITORY https://github.com/taskflow/taskflow.git
#        GIT_TAG v3.6.0)
#FetchContent_MakeAvailable(Taskflow)
#target_compile_definitions(${PROJECT_NAME} PUBLIC
#        TF_BUILD_EXAMPLES=OFF
#        TF_BUILD_TESTS=OFF)
#target_link_libraries(${PROJECT_NAME} PUBLIC
#        Taskflow)

###########
## TESTS ##
###########
add_executable(test examples/src/main.cpp)
target_link_libraries(test PRIVATE ${PROJECT_NAME})
