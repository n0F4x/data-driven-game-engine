target_sources(${PROJECT_NAME} PRIVATE
        Allocation.cpp
        Buffer.cpp
        copy_operations.cpp
        Image.cpp
        image_extensions.cpp
        mipmap_generation.cpp
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        Allocation.ixx
        Buffer.ixx
        copy_operations.ixx
        Image.ixx
        image_extensions.ixx
        MemoryTypeIndex.ixx
        MemoryView.ixx
        mipmap_generation.ixx
)
