target_sources(${PROJECT_NAME} PRIVATE
        adjust_snake_speed.cxx
        color_cells.cxx
        draw.cxx
        eat_apple.cxx
        game_is_running.cxx
        initialize.cxx
        move_snake.cxx
        shut_down.cxx
        spawn_apple.cxx
        trigger_world_update_message.cxx
        update.cxx
)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        tasks.cppm
        adjust_snake_speed.ixx
        color_cells.ixx
        eat_apple.ixx
        initialize.ixx
        update.ixx
        draw.ixx
        game_is_running.ixx
        move_snake.ixx
        shut_down.ixx
        spawn_apple.ixx
        trigger_world_update_message.ixx
)
