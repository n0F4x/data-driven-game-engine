target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        scheduler.cppm
        accessor_c.ixx
        as_task_builder_t.ixx
        converts_to_task_builder_c.ixx
        Nexus.ixx
        provide_accessors_for.ixx
        ProviderFor.ixx
        provides_c.ixx
        raw_task_c.ixx
        ScheduleBuilder.ixx
        Task.ixx
        TaskBuilder.ixx
        wrap_as_builder.ixx
)
