target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        concepts.cppm
        converts_to_task_builder_c.ixx
        converts_to_predicate_task_builder_c.ixx
        decays_to_task_builder_c.ixx
        predicate_task_builder_c.ixx
        provides_c.ixx
        repetition_specifier_task_builder.ixx
        task_builder_c.ixx
)
