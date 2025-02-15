target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ../../../../..
        FILES
        functional.cppm
        arguments_of.ixx
        invoke_result_of.ixx
        signature.ixx
)
