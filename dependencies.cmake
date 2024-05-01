set(BUILD_SHARED_LIBS OFF)

if (DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
endif ()

# Microsoft GSL
find_package(Microsoft.GSL CONFIG REQUIRED)
target_precompile_headers(${PROJECT_NAME} PRIVATE <gsl/gsl>)
target_link_libraries(${PROJECT_NAME} PUBLIC Microsoft.GSL::GSL)

# tl-optional
set(BUILD_TESTING OFF)
find_package(tl-optional CONFIG REQUIRED)
target_precompile_headers(${PROJECT_NAME} PRIVATE <tl/optional.hpp>)
target_link_libraries(${PROJECT_NAME} PUBLIC tl::optional)

# ordered_map
find_package(tsl-ordered-map CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC tsl::ordered_map)

# spdlog
find_package(spdlog CONFIG REQUIRED)
if (engine_debug)
    set(spdlog_level SPDLOG_LEVEL_TRACE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            SPDLOG_ACTIVE_LEVEL=${spdlog_level}
    )
endif ()
target_precompile_headers(${PROJECT_NAME} PRIVATE <spdlog/spdlog.h>)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)

# GLFW
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
find_package(glfw3 CONFIG REQUIRED)
target_compile_definitions(${PROJECT_NAME} PUBLIC
        GLFW_INCLUDE_VULKAN
)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

# Vulkan
message(NOTICE "Configuring Vulkan")
find_package(Vulkan REQUIRED)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        VULKAN_HPP_NO_TO_STRING
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
)
if (engine_debug)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENGINE_VULKAN_DEBUG)
endif ()
target_precompile_headers(${PROJECT_NAME} PRIVATE
        <vulkan/vulkan.hpp>
)
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)

# SPIRV-Cross
set(SPIRV_CROSS_STATIC ON)
set(SPIRV_CROSS_SHARED OFF)
set(SPIRV_CROSS_CLI OFF)
find_package(spirv_cross_core CONFIG REQUIRED)
find_package(spirv_cross_glsl CONFIG REQUIRED)
find_package(spirv_cross_reflect CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spirv-cross-core spirv-cross-glsl spirv-cross-reflect)

# VulkanMemoryAllocator
# vcpkg port(3.0.1#4) is deprecated and doesn't compile
message(NOTICE "Configuring VulkanMemoryAllocator")
set(STATIC_VULKAN_FUNCTIONS 0)
set(DYNAMIC_VULKAN_FUNCTIONS 1)
FetchContent_Declare(VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG 19b940e864bd3a5afb3c79e3c6788869d01a19eb
        GIT_PROGRESS TRUE
        SYSTEM
)
FetchContent_MakeAvailable(VulkanMemoryAllocator)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        VMA_STATIC_VULKAN_FUNCTIONS=${STATIC_VULKAN_FUNCTIONS}
        VMA_DYNAMIC_VULKAN_FUNCTIONS=${DYNAMIC_VULKAN_FUNCTIONS}
)
target_link_libraries(${PROJECT_NAME} PUBLIC GPUOpen::VulkanMemoryAllocator)

# vk-bootstrap
find_package(vk-bootstrap CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC vk-bootstrap::vk-bootstrap)

# glm
find_package(glm CONFIG REQUIRED)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        GLM_FORCE_DEPTH_ZERO_TO_ONE
)
target_precompile_headers(${PROJECT_NAME} PRIVATE <glm/glm.hpp>)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)

# KTX
set(KTX_FEATURE_STATIC_LIBRARY ON)
find_package(Ktx CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC KTX::ktx)

# stb
find_package(Stb REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Stb_INCLUDE_DIR})

# fastgltf
find_package(fastgltf CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC fastgltf::fastgltf)

# EnTT
find_package(EnTT CONFIG REQUIRED)
target_precompile_headers(${PROJECT_NAME} PRIVATE <entt/entt.hpp>)
target_link_libraries(${PROJECT_NAME} PUBLIC EnTT::EnTT)
