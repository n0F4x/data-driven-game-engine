target_sources(${PROJECT_NAME} PRIVATE
        stb_image_impl.c
        stb_image_resize2_impl.c
        vma_impl.cpp
        vulkan.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        config.cppm
        logging.ixx
        vulkan.ixx
)
