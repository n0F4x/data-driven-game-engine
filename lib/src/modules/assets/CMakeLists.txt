target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        assets.cppm
        asset_c.ixx
        AssetManager.ixx
        Cached.ixx
        Handle.ixx
        loader_c.ixx
        WeakHandle.ixx
)
