target_sources(${PROJECT_NAME} PRIVATE
        create_eat_apple_task_builder.cxx
        create_initialize_task_builder.cxx
        create_update_task_builder.cxx
)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        tasks.cppm
        adjust_snake_speed.ixx
        color_cells.ixx
        create_eat_apple_task_builder.ixx
        create_initialize_task_builder.ixx
        create_update_task_builder.ixx
        draw.ixx
        game_is_running.ixx
        move_snake.ixx
        setup.ixx
        shut_down.ixx
        spawn_apple.ixx
        trigger_world_update_message.ixx
)
