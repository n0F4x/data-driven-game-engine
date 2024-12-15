target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        functional.cppm
        arguments_of.ixx
        concepts.ixx
        invoke_result_of.ixx
        signature.ixx
)
