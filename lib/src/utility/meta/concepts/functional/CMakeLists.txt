target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        functional.cppm
        function.ixx
        function_pointer.ixx
        function_reference.ixx
        member_function_pointer.ixx
        unambiguous_functor.ixx
        unambiguously_invocable.ixx
)
