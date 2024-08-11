add_subdirectory(extensions)

target_sources(${PROJECT_NAME} PRIVATE
        ImageLoader.cpp
        Loader.cpp
        Material.cpp
        Model.cpp
        Node.cpp
        Texture.cpp
)
