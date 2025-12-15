if (DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
endif ()

# magic_enum
find_package(magic_enum CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC magic_enum::magic_enum)

# tl-function-ref
find_package(tl-function-ref REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC tl::function-ref)

# tsl-ordered_map
find_package(tsl-ordered-map CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC tsl::ordered_map)

# fmt
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)

# spdlog
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)

# Vulkan
find_package(VulkanHeaders CONFIG REQUIRED)
get_target_property(VulkanHeaders_INCLUDE_DIRS Vulkan::Headers INTERFACE_INCLUDE_DIRECTORIES)
add_library(VulkanCppModule)
target_compile_features(VulkanCppModule PUBLIC cxx_std_20)
target_sources(VulkanCppModule PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${VulkanHeaders_INCLUDE_DIRS}
        FILES ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vulkan.cppm
)
target_compile_definitions(VulkanCppModule PUBLIC
        VK_NO_PROTOTYPES
        VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS
        VULKAN_HPP_NO_EXCEPTIONS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
)
target_link_libraries(VulkanCppModule PUBLIC Vulkan::Headers)
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Headers)
target_link_libraries(${PROJECT_NAME} PUBLIC $<BUILD_LOCAL_INTERFACE:VulkanCppModule>)

# EnTT
find_package(EnTT CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC EnTT::EnTT)
