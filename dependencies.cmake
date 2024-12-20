include(FetchContent)
set(BUILD_SHARED_LIBS OFF)


if (DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
endif ()

# gsl-lite
fetchcontent_declare(gsl-lite
        GIT_REPOSITORY https://github.com/gsl-lite/gsl-lite.git
        GIT_TAG a8c7e5bbbd08841836f9b92d72747fb8769dbec4
        SYSTEM
)
fetchcontent_makeavailable(gsl-lite)
target_link_libraries(${PROJECT_NAME} PUBLIC gsl::gsl-lite-v1)

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
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)

# GLFW
find_package(glfw3 CONFIG REQUIRED)
target_compile_definitions(${PROJECT_NAME} PUBLIC
        GLFW_INCLUDE_VULKAN
)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

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
)
target_compile_definitions(VulkanHppModule PUBLIC
        VULKAN_HPP_NO_TO_STRING
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
)
target_compile_features(VulkanHppModule PUBLIC cxx_std_26)
target_link_libraries(VulkanHppModule PUBLIC Vulkan::Headers)

target_link_libraries(${PROJECT_NAME} PUBLIC VulkanHppModule)
if (engine_debug)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENGINE_VULKAN_DEBUG)
endif ()

# Vulkan-Utility
find_package(VulkanUtilityLibraries CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::UtilityHeaders)

# VulkanMemoryAllocator
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
set(VMA_STATIC_FUNCTIONS 0)
set(VMA_DYNAMIC_FUNCTIONS 0)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        VMA_STATIC_VULKAN_FUNCTIONS=${VMA_STATIC_FUNCTIONS}
        VMA_DYNAMIC_VULKAN_FUNCTIONS=${VMA_STATIC_FUNCTIONS}
)
target_link_libraries(${PROJECT_NAME} PUBLIC GPUOpen::VulkanMemoryAllocator)

# vk-bootstrap
FetchContent_Declare(vk-bootstrap
        GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap.git
        GIT_TAG v1.3.296
        SYSTEM
)
FetchContent_MakeAvailable(vk-bootstrap)
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
add_subdirectory(external/stb_image)
target_link_libraries(${PROJECT_NAME} PUBLIC stb_image)
add_subdirectory(external/stb_image_resize2)
target_link_libraries(${PROJECT_NAME} PUBLIC stb_image_resize2)

# fastgltf
# TODO: enable modules
set(FASTGLTF_COMPILE_AS_CPP20 ON)
FetchContent_Declare(fastgltf
        GIT_REPOSITORY https://github.com/spnda/fastgltf.git
        GIT_TAG c462eaf7114f16a977afe84d0a4590b33091a33f # after v0.8.0
        SYSTEM
)
FetchContent_MakeAvailable(fastgltf)
target_link_libraries(${PROJECT_NAME} PUBLIC fastgltf::fastgltf)

# EnTT
find_package(EnTT CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC EnTT::EnTT)
