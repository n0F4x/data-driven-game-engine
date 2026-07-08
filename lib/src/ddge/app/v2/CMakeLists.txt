add_subdirectory(config)
add_subdirectory(registry)
add_subdirectory(memory)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        v2.cppm
        App.ixx
        Builder.ixx
        create.ixx
)
