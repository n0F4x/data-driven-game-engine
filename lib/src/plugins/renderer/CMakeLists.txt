target_sources(${PROJECT_NAME} PRIVATE
        DevicePlugin.cxx
        InstancePlugin.cxx
        SurfacePlugin.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        renderer.cppm
        DevicePlugin.ixx
        InstancePlugin.ixx
        Renderer.ixx
        SurfacePlugin.ixx
)
