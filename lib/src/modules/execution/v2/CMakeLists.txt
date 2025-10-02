add_subdirectory(primitives)

target_sources(${PROJECT_NAME} PRIVATE
        ScheduleBuilder.cxx
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
        ExecPolicy.ixx
        ScheduleBuilder.ixx
        Task.ixx
        TaskBuilder.ixx
        TaskFinishedCallback.ixx
        TaskHub.ixx
        TaskHubBuilder.ixx
        TaskHubProxy.ixx
        TaskIndex.ixx
)
