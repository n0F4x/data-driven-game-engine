cmake_minimum_required(VERSION 3.25)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(engine)


add_library(${PROJECT_NAME})


option(engine_debug "Turn on debug mode for library" ON)


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


#target_compile_definitions(${PROJECT_NAME} PUBLIC ENGINE_DEBUG)
#get_target_property(engine_compile_defs ${PROJECT_NAME} COMPILE_DEFINITIONS)


target_include_directories(${PROJECT_NAME} PUBLIC
        src
        )

add_subdirectory(src/engine)


# optional_view
add_subdirectory(vendor/optional_view)
target_link_libraries(${PROJECT_NAME} PUBLIC
        optional_view)

## Microsoft.GSL
#FetchContent_Declare(Microsoft.GSL
#        GIT_REPOSITORY https://github.com/microsoft/GSL.git
#        GIT_TAG v4.0.0
#        GIT_PROGRESS TRUE)
#FetchContent_MakeAvailable(Microsoft.GSL)
#target_link_libraries(${PROJECT_NAME} PUBLIC
#        Microsoft.GSL::GSL)

# spdlog
FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.12.0
        GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(spdlog)
if (engine_debug)
    set(spdlog_level SPDLOG_LEVEL_TRACE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
            SPDLOG_ACTIVE_LEVEL=${spdlog_level}
            )
endif ()
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)

# SFML
set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.0
        GIT_PROGRESS TRUE)
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
if (engine_debug)
    target_compile_definitions(${PROJECT_NAME} PUBLIC ENGINE_VULKAN_DEBUG)
endif ()
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)

# VulkanMemoryAllocator
set(STATIC_VULKAN_FUNCTIONS 0)
set(DYNAMIC_VULKAN_FUNCTIONS 1)
add_subdirectory(libs/vulkan-memory-allocator)
target_compile_definitions(vulkan-memory-allocator PUBLIC
        VMA_STATIC_VULKAN_FUNCTIONS=${STATIC_VULKAN_FUNCTIONS}
        VMA_DYNAMIC_VULKAN_FUNCTIONS=${DYNAMIC_VULKAN_FUNCTIONS}
        )
target_link_libraries(${PROJECT_NAME} PUBLIC
        vulkan-memory-allocator)

# glm
FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8
        GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(glm)
target_compile_definitions(${PROJECT_NAME} PUBLIC
        GLM_FORCE_DEPTH_ZERO_TO_ONE
        )
target_link_libraries(${PROJECT_NAME} PUBLIC
        glm::glm)

# tinygltf
FetchContent_Declare(tinygltf
        GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
        GIT_TAG v2.8.17
        GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(tinygltf)
target_compile_definitions(${PROJECT_NAME} PUBLIC
        TINYGLTF_NOEXCEPTION
        TINYGLTF_USE_CPP14
        )
target_link_libraries(${PROJECT_NAME} PUBLIC
        tinygltf)

# EnTT
FetchContent_Declare(EnTT
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.12.2
        GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(EnTT)
target_link_libraries(${PROJECT_NAME} PUBLIC
        EnTT)

## Taskflow
#FetchContent_Declare(Taskflow
#        GIT_REPOSITORY https://github.com/taskflow/taskflow.git
#        GIT_TAG v3.6.0
#        GIT_PROGRESS TRUE)
#FetchContent_MakeAvailable(Taskflow)
#target_compile_definitions(${PROJECT_NAME} PUBLIC
#        TF_BUILD_EXAMPLES=OFF
#        TF_BUILD_TESTS=OFF)
#target_link_libraries(${PROJECT_NAME} PUBLIC
#        Taskflow)


unset(engine_debug CACHE)


###########
## TESTS ##
###########
add_executable(test examples/src/main.cpp)
target_link_libraries(test PUBLIC ${PROJECT_NAME})
