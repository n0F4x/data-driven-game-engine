target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        game.cppm
        Cell.ixx
        Charge.ixx
        Direction.ixx
        draw.ixx
        Position.ixx
        setup.ixx
        Snake.ixx
        SnakeHead.ixx
)
