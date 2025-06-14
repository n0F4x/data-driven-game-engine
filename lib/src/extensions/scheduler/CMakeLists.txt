add_subdirectory(accessors)
add_subdirectory(argument_providers)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        scheduler.cppm
        argument_provider_c.ixx
        argument_provider_for.ixx
)
