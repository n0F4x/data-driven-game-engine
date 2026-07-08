target_sources(${PROJECT_NAME} PRIVATE
        Requirements.cxx
        SwapchainHolder.cxx
        Swapchain.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        swapchain.cppm
        SwapchainHolder.ixx
        Swapchain.ixx
)
