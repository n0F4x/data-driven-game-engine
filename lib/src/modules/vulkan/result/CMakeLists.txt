target_sources(${PROJECT_NAME} PRIVATE
        VulkanError.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        result.cppm
        check_result.ixx
        make_typed_result_code_variant.ixx
        Result.ixx
        result_category.ixx
        result_description.ixx
        ResultCategory.ixx
        TypedResultCode.ixx
        VulkanError.ixx
)
