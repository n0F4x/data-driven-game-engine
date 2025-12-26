target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        wsi.cppm
        Context.ixx
        Error.ixx
        FullScreenWindowSettings.ixx
        monitors.ixx
        Size.ixx
        vulkan_instance_extensions.ixx
        VulkanWindow.ixx
        Window.ixx
        WindowedWindowSettings.ixx
        WindowSettings.ixx
)
