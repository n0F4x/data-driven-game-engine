target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        primitives.cppm
        all_of.ixx
        at_fixed_rate.ixx
        group.ixx
        in_parallel.ixx
        loop_until.ixx
        repeat.ixx
        run_if.ixx
        start_as.ixx
)
