add_subdirectory(gatherers)
add_subdirectory(primitives)

target_sources(${PROJECT_NAME} PRIVATE
        TaskHub.cxx
        TaskHubProxy.cxx
        TaskHubBuilder.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        v2.cppm
        as_task_blueprint.ixx
        Cardinality.ixx
        convertible_to_TaskBlueprint_c.ixx
        EmbeddedTaskBody.ixx
        EmbeddedTaskFactory.ixx
        ErasedTaskFactory.ixx
        ExecPolicy.ixx
        ScheduleBuilder.ixx
        Task.ixx
        TaskBlueprint.ixx
        TaskBuilder.ixx
        TaskBuilderBundle.ixx
        TaskBundle.ixx
        TaskContinuation.ixx
        TaskContinuationFactory.ixx
        TaskFinishedCallback.ixx
        TaskHub.ixx
        TaskHubBuilder.ixx
        TaskHubProxy.ixx
        TaskIndex.ixx
)
