target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        task_wrappers.cppm
        LoopUntil.ixx
        Schedule.ixx
        TaskGroup.ixx
        TaskWrapperBase.ixx
)
