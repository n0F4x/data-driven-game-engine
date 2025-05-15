target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        task_builders.cppm
        LoopUntilBuilder.ixx
        ScheduleBuilder.ixx
        SimpleTaskBuilder.ixx
        TaskBuilderBase.ixx
        InParallelBuilder.ixx
)
