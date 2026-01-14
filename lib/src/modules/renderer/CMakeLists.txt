target_sources(${PROJECT_NAME} PRIVATE
        ContextInjection.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        renderer.cppm
        Plugin.ixx
        Context.ixx
        ContextInjection.ixx
        vulkan_profile.ixx
)
