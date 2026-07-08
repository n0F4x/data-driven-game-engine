add_subdirectory(app)
add_subdirectory(assets)
add_subdirectory(config)
add_subdirectory(deprecated)
add_subdirectory(ecs)
add_subdirectory(events)
add_subdirectory(log)
add_subdirectory(messages)
add_subdirectory(prelude)
add_subdirectory(resources)
add_subdirectory(scheduler)
add_subdirectory(states)
add_subdirectory(time)
add_subdirectory(util)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        ddge.cppm
)
