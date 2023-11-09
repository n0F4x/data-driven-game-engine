add_subdirectory(renderer)
add_subdirectory(asset_manager)
add_subdirectory(graphics)
add_subdirectory(scene_graph)
add_subdirectory(window)

target_sources(${PROJECT_NAME} PRIVATE
        Logger.cpp
        Renderer.cpp
        AssetManager.cpp
        SceneGraph.cpp
        Window.cpp
)
