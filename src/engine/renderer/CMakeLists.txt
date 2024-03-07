add_subdirectory(base)
add_subdirectory(material_system)
add_subdirectory(model)

target_sources(${PROJECT_NAME} PRIVATE
        Plugin.cpp
        Renderer.cpp
        ResourceManager.cpp
        Scene.cpp
        SceneBuilder.cpp
)
