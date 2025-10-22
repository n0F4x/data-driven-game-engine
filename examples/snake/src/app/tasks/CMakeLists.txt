target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        tasks.cppm
        clear_messages.ixx
        initialize.ixx
        process_events.ixx
        render.ixx
        run_game_loop.ixx
        shut_down.ixx
        update.ixx
)
