add_subdirectory(scene)

target_sources(${PROJECT_NAME} PRIVATE
        Allocator.cpp
        Device.cpp
        Plugin.cpp
        helpers.cpp
        Instance.cpp
        RenderFrame.cpp
        Swapchain.cpp
)
