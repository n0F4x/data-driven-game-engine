target_sources(${PROJECT_NAME} PRIVATE
        DevicePlugin.cpp
        InstancePlugin.cpp
        SurfacePlugin.cpp
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        DevicePlugin.ixx
        InstancePlugin.ixx
        SurfacePlugin.ixx
)
