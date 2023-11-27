add_subdirectory(scene)

target_sources(${PROJECT_NAME} PRIVATE
        Device.cpp
        Plugin.cpp
        helpers.cpp
        Instance.cpp
        RenderFrame.cpp
        Swapchain.cpp
)
