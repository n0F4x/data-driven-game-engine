target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        assets.cppm
        inject_loaders.ixx
        root_direcory.ixx
        TextureLoader.ixx
)
