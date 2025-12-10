add_subdirectory(format)
add_subdirectory(result)

target_sources(${PROJECT_NAME} PRIVATE
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
        minimum_vulkan_api_version.ixx
)
