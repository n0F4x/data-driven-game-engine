target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        game.cppm
        Cell.ixx
        color_cells.ixx
        Direction.ixx
        draw.ixx
        game_is_running.ixx
        game_tick_rate.ixx
        GameOver.ixx
        move_snake.ixx
        Position.ixx
        Settings.ixx
        setup.ixx
        initialize.ixx
        Snake.ixx
        SnakeHead.ixx
        update.ixx
)
