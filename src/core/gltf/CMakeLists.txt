add_subdirectory(extensions)
add_subdirectory(shaders)

target_sources(${PROJECT_NAME} PRIVATE
        ImageLoader.cpp
        Loader.cpp
        Material.cpp
        Model.cpp
        Node.cpp
        RenderModel.cpp
        Texture.cpp
)
