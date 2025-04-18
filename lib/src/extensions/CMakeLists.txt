target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        extensions.cppm
        AddonManager.ixx
        Functional.ixx
        ResourceManager.ixx
        Runnable.ixx
        ScheduleRunner.ixx
)
