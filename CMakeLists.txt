cmake_minimum_required(VERSION 3.25)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

project(engine CXX)


add_library(${PROJECT_NAME})


option(engine_debug "Turn on debug mode for library" ON)


target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)


if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
            /W4
            /WX
    )
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall
            -Wextra
            -Werror
            -Wno-missing-field-initializers
    )
endif ()


target_precompile_headers(${PROJECT_NAME} PRIVATE
        <algorithm>
        <concepts>
        <expected>
        <fstream>
        <functional>
        <iostream>
        <limits>
        <memory>
        <ranges>
        <set>
        <span>
        <sstream>
        <stdexcept>
        <string>
        <string_view>
        <thread>
        <type_traits>
        <unordered_map>
        <utility>
        <vector>
        <variant>
)


# tl-optional
set(BUILD_TESTING OFF)
FetchContent_Declare(tl-optional
        GIT_REPOSITORY https://github.com/TartanLlama/optional.git
        GIT_TAG v1.1.0
        GIT_PROGRESS TRUE
        SYSTEM
)
FetchContent_MakeAvailable(tl-optional)
target_precompile_headers(${PROJECT_NAME} PRIVATE <tl/optional.hpp>)
target_link_libraries(${PROJECT_NAME} PUBLIC tl::optional)

# spdlog
FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.12.0
        GIT_PROGRESS TRUE
        SYSTEM
)
FetchContent_MakeAvailable(spdlog)
if (engine_debug)
    set(spdlog_level SPDLOG_LEVEL_TRACE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            SPDLOG_ACTIVE_LEVEL=${spdlog_level}
    )
endif ()
target_precompile_headers(${PROJECT_NAME} PRIVATE <spdlog/spdlog.h>)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)

# SFML
set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
        GIT_PROGRESS TRUE
        SYSTEM
)
FetchContent_MakeAvailable(SFML)
target_link_libraries(${PROJECT_NAME} PUBLIC sfml-window)

# Vulkan
find_package(Vulkan REQUIRED)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        VULKAN_HPP_NO_EXCEPTIONS
        VULKAN_HPP_NO_TO_STRING
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
)
if (engine_debug)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENGINE_VULKAN_DEBUG)
endif ()
target_precompile_headers(${PROJECT_NAME} PRIVATE
        src/engine/config/vulkan_pre_macros.hpp
        <vulkan/vulkan.hpp>
)
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)

# VulkanMemoryAllocator
set(STATIC_VULKAN_FUNCTIONS 0)
set(DYNAMIC_VULKAN_FUNCTIONS 1)
FetchContent_Declare(VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG 2f382df
        GIT_PROGRESS TRUE
        SYSTEM
)
FetchContent_MakeAvailable(VulkanMemoryAllocator)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        VMA_STATIC_VULKAN_FUNCTIONS=${STATIC_VULKAN_FUNCTIONS}
        VMA_DYNAMIC_VULKAN_FUNCTIONS=${DYNAMIC_VULKAN_FUNCTIONS}
)
target_link_libraries(${PROJECT_NAME} PUBLIC VulkanMemoryAllocator)

# glm
FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8
        GIT_PROGRESS TRUE
        SYSTEM
)
FetchContent_MakeAvailable(glm)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        GLM_FORCE_DEPTH_ZERO_TO_ONE
)
target_precompile_headers(${PROJECT_NAME} PRIVATE <glm/glm.hpp>)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)

# tinygltf
FetchContent_Declare(tinygltf
        GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
        GIT_TAG v2.8.17
        GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(tinygltf)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        TINYGLTF_NOEXCEPTION
        TINYGLTF_USE_CPP14
)
target_link_libraries(${PROJECT_NAME} PUBLIC tinygltf)

# EnTT
FetchContent_Declare(EnTT
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.12.2
        GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(EnTT)
target_precompile_headers(${PROJECT_NAME} PRIVATE <entt/entt.hpp>)
target_link_libraries(${PROJECT_NAME} PUBLIC EnTT)


target_include_directories(${PROJECT_NAME} PUBLIC src)

add_subdirectory(src)


unset(engine_debug CACHE)


###########
## TESTS ##
###########
add_subdirectory(example)
