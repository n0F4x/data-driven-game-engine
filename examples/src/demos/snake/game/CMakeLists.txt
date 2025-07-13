add_subdirectory(functions)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        game.cppm
        Apple.ixx
        apple_spawn_rate.ixx
        Cell.ixx
        Direction.ixx
        game_tick_rate.ixx
        GameOver.ixx
        GameState.ixx
        Position.ixx
        Settings.ixx
        Snake.ixx
        SnakeHead.ixx
)
