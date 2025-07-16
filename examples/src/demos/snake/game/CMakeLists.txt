add_subdirectory(events)
add_subdirectory(functions)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        game.cppm
        Apple.ixx
        apple_spawn_time.ixx
        AppleSpawnTimer.ixx
        Cell.ixx
        Direction.ixx
        GameState.ixx
        Position.ixx
        Settings.ixx
        Snake.ixx
        SnakeHead.ixx
)
