target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        gatherers.cppm
        AllOf.ixx
        gatherer_builder_of_c.ixx
        gatherer_of_c.ixx
        WaitAll.ixx
)
