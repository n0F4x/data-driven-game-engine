target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        extension_inspection.cppm
        extension_is_promoted_to_vulkan1x.ixx
        extensions_promoted_to_vulkan1x.ixx
        try_promote_extension_to_vulkan1x.ixx
)
