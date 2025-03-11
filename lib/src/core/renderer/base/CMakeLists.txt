add_subdirectory(allocator)
add_subdirectory(resources)
add_subdirectory(descriptor_pool)
add_subdirectory(device)
add_subdirectory(instance)
add_subdirectory(swapchain)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        base.cppm
)
