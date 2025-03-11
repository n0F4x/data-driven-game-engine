target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        functional.cppm
        arguments_of.ixx
        invoke_result_of.ixx
        signature.ixx
)
