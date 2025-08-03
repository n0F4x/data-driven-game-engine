add_subdirectory(accessors)
add_subdirectory(primitives)
add_subdirectory(providers)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        scheduler.cppm
        provider_c.ixx
        ProviderOf.ixx
)
