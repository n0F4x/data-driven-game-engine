target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ../../../../..
        FILES
        functional.cppm
        callable_c.ixx
        function_c.ixx
        functor_c.ixx
        member_function_pointer_c.ixx
)
