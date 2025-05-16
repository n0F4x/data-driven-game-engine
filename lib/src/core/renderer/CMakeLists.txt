add_subdirectory(base)
add_subdirectory(material_system)
add_subdirectory(model)
add_subdirectory(resources)
add_subdirectory(scene)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        renderer.cppm
)
