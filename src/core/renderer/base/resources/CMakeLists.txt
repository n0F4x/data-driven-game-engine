target_sources(${PROJECT_NAME} PRIVATE
        Allocation.cxx
        Buffer.cxx
        copy_operations.cxx
        Image.cxx
        image_extensions.cxx
        mipmap_generation.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        resources.cppm
        Allocation.ixx
        Buffer.ixx
        copy_operations.ixx
        Image.ixx
        image_extensions.ixx
        MemoryTypeIndex.ixx
        MemoryView.ixx
        mipmap_generation.ixx
)
