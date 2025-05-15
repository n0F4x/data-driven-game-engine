add_subdirectory(shaders)

target_sources(${PROJECT_NAME} PRIVATE
        ImageLoader.cxx
        Loader.cxx
        Material.cxx
        Model.cxx
        Node.cxx
        RenderModel.cxx
        Texture.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        gltf.cppm
        Image.ixx
        ImageLoader.ixx
        Material.ixx
        Mesh.ixx
        Model.ixx
        RenderModel.ixx
        Texture.ixx
)
