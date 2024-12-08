target_sources(${PROJECT_NAME} PRIVATE
        vma_impl.cpp
        vulkan.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        vulkan.ixx
)
