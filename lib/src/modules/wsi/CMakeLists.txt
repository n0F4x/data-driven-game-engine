target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        wsi.cppm
        Context.ixx
        Error.ixx
        monitors.ixx
        Resolution.ixx
        vulkan_instance_extensions.ixx
        Window.ixx
)
