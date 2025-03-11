target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        functional.cppm
        callable.ixx
        function.ixx
        functor.ixx
        member_function_pointer.ixx
)
