target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        concepts.cppm
        converts_to_task_c.ixx
        converts_to_task_predicate_c.ixx
        decays_to_task_c.ixx
        provides_dependency_c.ixx
        task_c.ixx
        task_predicate_c.ixx
        task_wrapper_c.ixx
)
