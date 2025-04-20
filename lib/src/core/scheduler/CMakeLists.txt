add_subdirectory(concepts)
add_subdirectory(task_wrappers)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        scheduler.cppm
        as_task.ixx
        group.ixx
        invoke_with_right_dependency_providers.ixx
        loop_until.ixx
        TaskRunner.ixx
        start_as.ixx
        Task.ixx
)
