add_subdirectory(vma)

target_sources(${PROJECT_NAME} PRIVATE
        result_types.cpp
        Swapchain.cpp
        tools.cpp
)
