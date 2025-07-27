project(snake)

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE engine::engine)

find_package(SFML CONFIG REQUIRED COMPONENTS Graphics)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics)

add_subdirectory(assets)
add_subdirectory(game)
add_subdirectory(window)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        snake.cppm
)

target_sources(${PROJECT_NAME} PRIVATE
        main.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE
        -ftime-trace=file.json
)
