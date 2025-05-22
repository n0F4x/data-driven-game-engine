target_sources(${PROJECT_NAME} PRIVATE
        stb_image_impl.c
        stb_image_resize2_impl.c
        vma_impl.cpp
        vulkan.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        config.cppm
        vulkan.ixx
)
