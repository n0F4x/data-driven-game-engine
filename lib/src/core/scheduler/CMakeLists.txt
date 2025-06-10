add_subdirectory(concepts)
add_subdirectory(task_builders)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        scheduler.cppm
        all_of.ixx
        as_task_builder.ixx
        at_fixed_rate.ixx
        build.ixx
        group.ixx
        in_parallel.ixx
        loop_until.ixx
        provider_index_for_argument.ixx
        repeat.ixx
        run_if.ixx
        start_as.ixx
)
