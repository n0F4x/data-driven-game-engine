target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        functional.cppm
        arguments_of.ixx
        is_noexcept.ixx
        result_of.ixx
        Signature.ixx
        signature_of.ixx
)
