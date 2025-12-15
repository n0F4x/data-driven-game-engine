add_subdirectory(format)
add_subdirectory(result)
add_subdirectory(structure_chains)

target_sources(${PROJECT_NAME} PRIVATE
        default_debug_messenger_callback.cxx
        global_definitions.cpp
        InstanceBuilder.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        vulkan.cppm
        context.ixx
        InstanceBuilder.ixx
        default_debug_messenger_callback.ixx
        DeviceBuilder.ixx
        minimum_vulkan_api_version.ixx
        PhysicalDeviceSelector.ixx
)
