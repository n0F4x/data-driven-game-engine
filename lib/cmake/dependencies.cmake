# TODO: use Conan local_recipes_index instead of fetch_content

include(FetchContent)

if (DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
endif ()


# gsl-lite
find_package(gsl-lite CONFIG REQUIRED)
target_compile_definitions(${PROJECT_NAME} PRIVATE gsl_CONFIG_DEFAULTS_VERSION=1)
target_link_libraries(${PROJECT_NAME} PRIVATE gsl::gsl-lite)

# tsl-ordered_map
find_package(tsl-ordered-map CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE tsl::ordered_map)

# fmt
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

# spdlog
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)

# GLFW
find_package(glfw3 CONFIG REQUIRED)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        GLFW_INCLUDE_VULKAN
)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# Vulkan
find_package(VulkanHeaders CONFIG REQUIRED)
get_target_property(VulkanHeaders_INCLUDE_DIRS Vulkan::Headers INTERFACE_INCLUDE_DIRECTORIES)
add_library(VulkanHppModule)
target_sources(VulkanHppModule PUBLIC
        FILE_SET CXX_MODULES
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
target_link_libraries(${PROJECT_NAME} PUBLIC VulkanHppModule) # TODO: make this PRIVATE

# Vulkan-Utility
fetchcontent_declare(VulkanUtilityLibraries
        GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Utility-Libraries.git
        GIT_TAG v1.3.296
        EXCLUDE_FROM_ALL
        SYSTEM
)
fetchcontent_makeavailable(VulkanUtilityLibraries)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::UtilityHeaders)

# VulkanMemoryAllocator
fetchcontent_declare(VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG v3.1.0
        EXCLUDE_FROM_ALL
        SYSTEM
)
fetchcontent_makeavailable(VulkanMemoryAllocator)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        VMA_STATIC_VULKAN_FUNCTIONS=0
        VMA_DYNAMIC_VULKAN_FUNCTIONS=0
)
target_link_libraries(${PROJECT_NAME} PUBLIC GPUOpen::VulkanMemoryAllocator) # TODO: make this PRIVATE

# vk-bootstrap
find_package(vk-bootstrap)
target_link_libraries(${PROJECT_NAME} PUBLIC vk-bootstrap::vk-bootstrap) # TODO: make this PRIVATE

# glm
find_package(glm CONFIG REQUIRED)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        GLM_FORCE_DEPTH_ZERO_TO_ONE
)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm) # TODO: make this PRIVATE

# KTX
find_package(Ktx CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE KTX::ktx)

# stb
find_package(stb CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE stb::stb)

# fastgltf
fetchcontent_declare(fastgltf
        GIT_REPOSITORY https://github.com/spnda/fastgltf.git
        GIT_TAG c462eaf7114f16a977afe84d0a4590b33091a33f # after v0.8.0
        EXCLUDE_FROM_ALL
        SYSTEM
)
# TODO: enable modules
set(FASTGLTF_COMPILE_AS_CPP20 ON)
fetchcontent_makeavailable(fastgltf)
target_compile_options(fastgltf PRIVATE
        -Wno-deprecated-literal-operator
        -Wno-sign-conversion
        -Wno-implicit-int-conversion
        -Wno-unused-parameter
        -Wno-old-style-cast
)
target_link_libraries(${PROJECT_NAME} PRIVATE fastgltf::fastgltf)

# EnTT
find_package(EnTT CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC EnTT::EnTT) # TODO: make this PRIVATE
