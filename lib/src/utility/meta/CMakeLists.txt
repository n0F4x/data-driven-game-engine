add_subdirectory(algorithms)
add_subdirectory(concepts)
add_subdirectory(reflection)
add_subdirectory(type_traits)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        meta.cppm
        int_at_least.ixx
        uint_at_least.ixx
)
