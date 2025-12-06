add_subdirectory(modules)
add_subdirectory(prelude)
add_subdirectory(utility)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        ddge.cppm
)
