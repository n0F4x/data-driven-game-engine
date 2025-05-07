add_subdirectory(concepts)
add_subdirectory(task_builders)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        scheduler.cppm
        as_task_builder.ixx
        build.ixx
        in_parallel.ixx
        loop_until.ixx
        provider_index_for_argument.ixx
        start_as.ixx
)
