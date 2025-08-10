add_subdirectory(app)
add_subdirectory(assets)
add_subdirectory(cache)
add_subdirectory(ecs)
add_subdirectory(events)
add_subdirectory(execution)
add_subdirectory(gfx)
add_subdirectory(gltf)
add_subdirectory(image)
add_subdirectory(log)
add_subdirectory(messages)
add_subdirectory(renderer)
add_subdirectory(resources)
add_subdirectory(states)
add_subdirectory(store)
add_subdirectory(time)
add_subdirectory(window)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        modules.cppm
)
