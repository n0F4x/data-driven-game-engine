target_sources(simple PRIVATE
        Registry.cxx
        Scheduler.cxx
        System.cxx
        SystemBuilder.cxx
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
        schedulable_c.ixx
        schedule_runner.ixx
        Scheduler.ixx
        SchedulerAddon.ixx
        System.ixx
        SystemBuilder.ixx
)
