add_subdirectory(extensions)

target_sources(${PROJECT_NAME} PRIVATE
        Model.cpp
        ImageLoader.cpp
        Model.cpp
        Loader.cpp
        Material.cpp
        Texture.cpp
)
