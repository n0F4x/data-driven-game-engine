target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        events.cppm
        BufferedEventQueue.ixx
        decays_to_event_c.ixx
        event_c.ixx
        EventManager.ixx
)
