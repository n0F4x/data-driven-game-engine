target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        plugins.cppm
        addons.ixx
        assets.ixx
        ecs.ixx
        events.ixx
        functional.ixx
        resources.ixx
        runnable.ixx
        scheduler.ixx
)
