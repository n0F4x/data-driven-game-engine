target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        concepts.cppm
        converts_to_task_builder_c.ixx
        converts_to_predicate_task_builder_c.ixx
        decays_to_task_builder_c.ixx
        predicate_task_builder_c.ixx
        provides_c.ixx
        task_builder_c.ixx
)
