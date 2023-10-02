add_subdirectory(renderer)
add_subdirectory(scene_graph)
add_subdirectory(window)

target_sources(${PROJECT_NAME} PRIVATE
        Renderer.cpp
        ResourceManager.cpp
        SceneGraph.cpp
        Window.cpp
        )
