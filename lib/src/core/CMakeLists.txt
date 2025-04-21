add_subdirectory(app)
add_subdirectory(cache)
add_subdirectory(config)
add_subdirectory(ecs)
add_subdirectory(events)
add_subdirectory(gfx)
add_subdirectory(gltf)
add_subdirectory(image)
add_subdirectory(log)
add_subdirectory(renderer)
add_subdirectory(resource)
add_subdirectory(scheduler)
add_subdirectory(store)
add_subdirectory(window)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        core.cppm
)
