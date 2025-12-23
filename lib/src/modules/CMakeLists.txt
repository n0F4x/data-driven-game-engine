add_subdirectory(app)
add_subdirectory(assets)
add_subdirectory(config)
add_subdirectory(ecs)
add_subdirectory(events)
add_subdirectory(log)
add_subdirectory(messages)
add_subdirectory(renderer)
add_subdirectory(resources)
add_subdirectory(scheduler)
add_subdirectory(states)
add_subdirectory(time)
add_subdirectory(vulkan)
add_subdirectory(wsi)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        modules.cppm
)
