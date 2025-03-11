add_subdirectory(base)
add_subdirectory(material_system)
add_subdirectory(model)
add_subdirectory(resources)
add_subdirectory(scene)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        renderer.cppm
)
