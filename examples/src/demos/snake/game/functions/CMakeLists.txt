target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        functions.cppm
        adjust_snake_speed.ixx
        color_cells.ixx
        draw.ixx
        eat_apple.ixx
        game_is_running.ixx
        initialize.ixx
        move_snake.ixx
        setup.ixx
        shut_down.ixx
        spawn_apple.ixx
        trigger_apple_digested_event.ixx
        update.ixx
)
