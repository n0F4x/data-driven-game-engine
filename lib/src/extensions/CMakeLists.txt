add_subdirectory(scheduler)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        extensions.cppm
        AddonManager.ixx
        EventManager.ixx
        Functional.ixx
        ResourceManager.ixx
        Runnable.ixx
        TaskRunner.ixx
)
