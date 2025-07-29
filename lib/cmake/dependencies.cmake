if (DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
endif ()


# gsl-lite
find_package(gsl-lite CONFIG REQUIRED)
target_compile_definitions(${PROJECT_NAME} PRIVATE gsl_CONFIG_DEFAULTS_VERSION=1)
target_link_libraries(${PROJECT_NAME} PUBLIC gsl::gsl-lite)

# tsl-ordered_map
find_package(tsl-ordered-map CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC tsl::ordered_map)

# fmt
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)

# spdlog
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)

# GLFW
find_package(glfw3 CONFIG REQUIRED)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        GLFW_INCLUDE_VULKAN
)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

# Vulkan
find_package(VulkanHeaders CONFIG REQUIRED)
get_target_property(VulkanHeaders_INCLUDE_DIRS Vulkan::Headers INTERFACE_INCLUDE_DIRECTORIES)
add_library(VulkanHppModule)
target_sources(VulkanHppModule PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${VulkanHeaders_INCLUDE_DIRS}
        FILES ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vulkan.cppm
)
target_compile_definitions(VulkanHppModule PUBLIC
        VK_NO_PROTOTYPES
        VULKAN_HPP_NO_TO_STRING
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
)
target_link_libraries(VulkanHppModule PUBLIC Vulkan::Headers)
target_link_libraries(${PROJECT_NAME} PUBLIC $<BUILD_LOCAL_INTERFACE:VulkanHppModule>)

# Vulkan-Utility
# TODO: use Vulkan-Hpp instead
include(FetchContent)
fetchcontent_declare(VulkanUtilityLibraries
        GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Utility-Libraries.git
        GIT_TAG v1.3.296
        EXCLUDE_FROM_ALL
        SYSTEM
)
fetchcontent_makeavailable(VulkanUtilityLibraries)
target_link_libraries(${PROJECT_NAME} PUBLIC $<BUILD_LOCAL_INTERFACE:Vulkan::UtilityHeaders>)

# VulkanMemoryAllocator
find_package(VulkanMemoryAllocator)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        VMA_STATIC_VULKAN_FUNCTIONS=0
        VMA_DYNAMIC_VULKAN_FUNCTIONS=0
)
target_link_libraries(${PROJECT_NAME} PUBLIC GPUOpen::VulkanMemoryAllocator)

# vk-bootstrap
find_package(vk-bootstrap)
target_link_libraries(${PROJECT_NAME} PUBLIC vk-bootstrap::vk-bootstrap)

# glm
find_package(glm CONFIG REQUIRED)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        GLM_FORCE_DEPTH_ZERO_TO_ONE
)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)

# KTX
find_package(Ktx CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC KTX::ktx)

# stb
find_package(stb CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC stb::stb)

# fastgltf
find_package(fastgltf)
target_link_libraries(${PROJECT_NAME} PUBLIC fastgltf::fastgltf)

# EnTT
find_package(EnTT CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC EnTT::EnTT)
