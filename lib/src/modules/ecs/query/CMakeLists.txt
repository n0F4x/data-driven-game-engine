target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        query.cppm
        component_query_filter_c.ixx
        OptionalView.ixx
        query_filter_c.ixx
        query_filter_components_are_all_different_c.ixx
        query_filter_tag_c.ixx
        query_filter_tags.ixx
        query_fn.ixx
        query_parameter_c.ixx
        queryable_component_c.ixx
        Query.ixx
        Query-fwd.ixx
        QueryFilterTagBase.ixx
        ToComponent.ixx
)
