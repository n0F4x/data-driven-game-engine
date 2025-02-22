target_sources(simple PRIVATE
        Task.cxx
)

target_sources(simple PRIVATE
        FILE_SET CXX_MODULES
        BASE_DIRS ${CURRENT_SOURCE_DIR}
        FILES
        ecs.cppm
        Query.ixx
        Registry.ixx
        RegistryAddon.ixx
        Res.ixx
        system_c.ixx
        schedule_runner.ixx
        Scheduler.ixx
        SchedulerAddon.ixx
        SchedulerAddonTag.ixx
        Task.ixx
        TaskBuilder.ixx
)
