add_subdirectory(vma)

target_sources(${PROJECT_NAME} PRIVATE
        Swapchain.cpp
        tools.cpp
)
