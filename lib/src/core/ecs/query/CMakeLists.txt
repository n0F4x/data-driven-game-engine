target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        query.cppm
        component_query_parameter_c.ixx
        OptionalView.ixx
        query.ixx
        query_parameter_c.ixx
        query_parameter_components_are_all_different_c.ixx
        query_parameter_tag_c.ixx
        query_parameter_tags.ixx
        query_parameter_tags-fwd.ixx
        queryable_component_c.ixx
        QueryAdaptorClosure.ixx
        QueryAdaptorClosure-fwd.ixx
        ToComponent.ixx
)
