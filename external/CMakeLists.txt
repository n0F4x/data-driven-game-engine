# Microsoft GSL
FetchContent_Declare(GSL
        GIT_REPOSITORY "https://github.com/microsoft/GSL"
        GIT_TAG "v4.0.0"
        GIT_SHALLOW ON
        GIT_PROGRESS TRUE
        SYSTEM
)
FetchContent_MakeAvailable(GSL)
target_precompile_headers(${PROJECT_NAME} PRIVATE <gsl/gsl>)
target_link_libraries(${PROJECT_NAME} PUBLIC Microsoft.GSL::GSL)

# tl-optional
message(NOTICE "Configuring tl-optional")
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
message(NOTICE "Configuring spdlog")
FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.13.0
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

# GLFW
message(NOTICE "Configuring GLFW")
FetchContent_Declare(GLFW
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.9
        GIT_PROGRESS TRUE
        SYSTEM
)
FetchContent_MakeAvailable(GLFW)
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
message(NOTICE "Configuring SPIRV-Cross")
FetchContent_Declare(SPIRV_Cross
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
        GIT_TAG vulkan-sdk-1.3.275.0
        GIT_PROGRESS TRUE
        SYSTEM
)
FetchContent_MakeAvailable(SPIRV_Cross)
target_link_libraries(${PROJECT_NAME} PRIVATE spirv-cross-cpp)

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
FetchContent_Declare(ktxlib
        GIT_REPOSITORY https://github.com/KhronosGroup/KTX-Software.git
        GIT_TAG v4.3.1
        GIT_PROGRESS TRUE
        SYSTEM
)
FetchContent_MakeAvailable(ktxlib)
target_link_libraries(${PROJECT_NAME} PUBLIC ktx)

# stb
message(NOTICE "Configuring stb")
add_subdirectory(stb SYSTEM)
target_link_libraries(${PROJECT_NAME} PUBLIC stb)

# fastgltf
message(NOTICE "Configuring fastgltf")
FetchContent_Declare(fastgltf
        GIT_REPOSITORY https://github.com/spnda/fastgltf.git
        GIT_TAG v0.7.0
        GIT_PROGRESS TRUE
        SYSTEM
)
FetchContent_MakeAvailable(fastgltf)
target_link_libraries(${PROJECT_NAME} PUBLIC fastgltf)

# EnTT
message(NOTICE "Configuring EnTT")
FetchContent_Declare(EnTT
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.13.0
        GIT_PROGRESS TRUE
        SYSTEM
)
FetchContent_MakeAvailable(EnTT)
target_precompile_headers(${PROJECT_NAME} PRIVATE <entt/entt.hpp>)
target_link_libraries(${PROJECT_NAME} PUBLIC EnTT)
