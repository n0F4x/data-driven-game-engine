add_subdirectory(vma)

target_sources(${PROJECT_NAME} PRIVATE
        raii_wrappers-Device.cpp
        raii_wrappers-Instance.cpp
        result_types.cpp
        Swapchain.cpp
        tools.cpp
)
