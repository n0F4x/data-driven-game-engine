add_subdirectory(scheduler)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        extensions.cppm
        AddonManager.ixx
        Assets.ixx
        EventManager.ixx
        Functional.ixx
        ResourceManager.ixx
        Runnable.ixx
        TaskRunner.ixx
)
