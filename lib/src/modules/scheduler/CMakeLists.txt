add_subdirectory(accessors)
add_subdirectory(data_structures)
add_subdirectory(gatherers)
add_subdirectory(locks)
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
        scheduler.cppm
        accessor_c.ixx
        EmbeddedTaskBody.ixx
        EmbeddedTaskFactory.ixx
        ErasedTaskFactory.ixx
        ExecPolicy.ixx
        IndirectTaskContinuationSetter.ixx
        IndirectTaskFactory.ixx
        Nexus.ixx
        Plugin.ixx
        provide_accessors_for.ixx
        provider_c.ixx
        ProviderFor.ixx
        ProviderOf.ixx
        provides_c.ixx
        raw_task_c.ixx
        ScheduleBuilder.ixx
        Task.ixx
        TaskBuilder.ixx
        TaskBuilderGroup.ixx
        TaskContinuation.ixx
        TaskContinuationFactory.ixx
        TaskFactory.ixx
        TaskHub.ixx
        TaskHubBuilder.ixx
        TaskHubProxy.ixx
        TaskIndex.ixx
        TypedTaskIndex.ixx
)
