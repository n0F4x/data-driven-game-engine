target_sources(${PROJECT_NAME} PRIVATE
        Builder.cxx
        DescriptorPool.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        descriptor_pool.cppm
        DescriptorPool.ixx
)
