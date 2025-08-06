target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        events.cppm
        BufferedEventQueue.ixx
        decays_to_event_c.ixx
        ErasedBufferedEventQueue.ixx
        event_c.ixx
        EventManager.ixx
)
