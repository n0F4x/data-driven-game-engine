add_subdirectory(assets)
add_subdirectory(ecs)
add_subdirectory(events)
add_subdirectory(resources)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        accessors.cppm
)
