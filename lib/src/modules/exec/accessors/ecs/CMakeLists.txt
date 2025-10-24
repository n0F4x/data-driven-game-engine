add_subdirectory(locks)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        ecs.cppm
        Provider.ixx
        Query.ixx
        Registry.ixx
)
