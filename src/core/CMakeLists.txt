add_subdirectory(app)
add_subdirectory(cache)
add_subdirectory(config)
add_subdirectory(ecs)
add_subdirectory(gfx)
add_subdirectory(gltf)
add_subdirectory(image)
add_subdirectory(renderer)
add_subdirectory(store)
add_subdirectory(window)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        core.cppm
)
