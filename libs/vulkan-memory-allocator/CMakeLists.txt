cmake_minimum_required(VERSION 3.25)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

project(vulkan-memory-allocator CXX)


add_library(${PROJECT_NAME})

# Vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)

# VulkanMemoryAllocator
FetchContent_Declare(VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG 2f382df
        GIT_PROGRESS TRUE
        SYSTEM
)
FetchContent_MakeAvailable(VulkanMemoryAllocator)
target_link_libraries(${PROJECT_NAME} PUBLIC
        VulkanMemoryAllocator
)

target_sources(${PROJECT_NAME} PRIVATE src/impl.cpp)
