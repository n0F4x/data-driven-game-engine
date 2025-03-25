target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        query.cppm
        Query.ixx
        Query-fwd.ixx
        query_parameter_tags.ixx
        queryable_component_c.ixx
)
