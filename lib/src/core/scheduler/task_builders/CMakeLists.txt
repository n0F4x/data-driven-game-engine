target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        task_builders.cppm
        AllOfTaskBuilder.ixx
        ConditionalTaskBuilder.ixx
        GroupedTaskBuilder.ixx
        LoopUntilBuilder.ixx
        RepeatedTaskBuilder.ixx
        ScheduleBuilder.ixx
        SimpleTaskBuilder.ixx
        TaskBuilderBase.ixx
        InParallelBuilder.ixx
)
