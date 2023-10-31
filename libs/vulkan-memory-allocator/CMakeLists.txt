cmake_minimum_required(VERSION 3.25)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(vulkan-memory-allocator CXX)


add_library(${PROJECT_NAME})


target_sources(${PROJECT_NAME} PRIVATE src/impl.cpp)


# Vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)

# VulkanMemoryAllocator
FetchContent_Declare(VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG 2f382df
        GIT_PROGRESS TRUE
        )
FetchContent_MakeAvailable(VulkanMemoryAllocator)
target_link_libraries(${PROJECT_NAME} PUBLIC
        VulkanMemoryAllocator)
