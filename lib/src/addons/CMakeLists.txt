target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        addons.cppm
        Assets.ixx
        ecs.ixx
        Events.ixx
        Resources.ixx
)
