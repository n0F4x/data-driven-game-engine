target_sources(${PROJECT_NAME} PRIVATE
        DeviceInjection.cxx
        InstanceInjection.cxx
        SurfaceInjection.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        renderer.cppm
        DeviceInjection.ixx
        InstanceInjection.ixx
        setup.ixx
        SurfaceInjection.ixx
)
