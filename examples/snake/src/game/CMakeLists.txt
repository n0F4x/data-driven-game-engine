add_subdirectory(events)
add_subdirectory(messages)
add_subdirectory(tasks)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        game.cppm
        Apple.ixx
        apple_spawn_duration.ixx
        AppleSpawnTimer.ixx
        Cell.ixx
        Direction.ixx
        GameState.ixx
        Position.ixx
        Settings.ixx
        setup.ixx
        Snake.ixx
        SnakeHead.ixx
)
