target_sources(${PROJECT_NAME} PRIVATE
        Requirements.cpp
        SwapchainHolder.cpp
        Swapchain.cpp
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        SwapchainHolder.ixx
        Swapchain.ixx
)
