set(BUILD_SHARED_LIBS OFF)

# Microsoft GSL
find_package(Microsoft.GSL)
target_precompile_headers(${PROJECT_NAME} PRIVATE <gsl/gsl>)
target_link_libraries(${PROJECT_NAME} PUBLIC Microsoft.GSL::GSL)

# tl-optional
set(BUILD_TESTING OFF)
find_package(tl-optional)
target_precompile_headers(${PROJECT_NAME} PRIVATE <tl/optional.hpp>)
target_link_libraries(${PROJECT_NAME} PUBLIC tl::optional)

# spdlog
find_package(spdlog)
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
find_package(glfw3)
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
find_package(spirv-cross)
target_link_libraries(${PROJECT_NAME} PRIVATE spirv-cross::spirv-cross)

# VulkanMemoryAllocator
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
target_link_libraries(${PROJECT_NAME} PUBLIC VulkanMemoryAllocator)

# glm
message(NOTICE "Configuring glm")
FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
        GIT_PROGRESS TRUE
        SYSTEM
)
FetchContent_MakeAvailable(glm)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        GLM_FORCE_DEPTH_ZERO_TO_ONE
)
target_precompile_headers(${PROJECT_NAME} PRIVATE <glm/glm.hpp>)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)

# KTX
message(NOTICE "Configuring KTX")
set(KTX_FEATURE_STATIC_LIBRARY ON)
FetchContent_Declare(ktxlib
        GIT_REPOSITORY https://github.com/KhronosGroup/KTX-Software.git
        GIT_TAG v4.3.1
        GIT_PROGRESS TRUE
        SYSTEM
)
FetchContent_MakeAvailable(ktxlib)
target_link_libraries(${PROJECT_NAME} PUBLIC ktx)

# stb
find_package(stb)
target_link_libraries(${PROJECT_NAME} PUBLIC stb::stb)

# fastgltf
find_package(fastgltf)
target_link_libraries(${PROJECT_NAME} PUBLIC fastgltf::fastgltf)

# EnTT
find_package(EnTT)
target_precompile_headers(${PROJECT_NAME} PRIVATE <entt/entt.hpp>)
target_link_libraries(${PROJECT_NAME} PUBLIC EnTT::EnTT)
