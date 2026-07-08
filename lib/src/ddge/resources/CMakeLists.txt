target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        resources.cppm
        Addon.ixx
        Plugin.ixx
        resource_c.ixx
        ResourceManager.ixx
)
