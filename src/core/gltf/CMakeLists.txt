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

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        Image.ixx
        ImageLoader.ixx
        Material.ixx
        Mesh.ixx
        Model.ixx
        RenderModel.ixx
        Texture.ixx
)
