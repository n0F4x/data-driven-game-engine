target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        functional.cppm
        function.ixx
        function_pointer.ixx
        function_reference.ixx
        member_function_pointer.ixx
        unambiguous_explicit_functor.ixx
        unambiguous_functor.ixx
        unambiguous_implicit_functor.ixx
        unambiguously_invocable.ixx
)
