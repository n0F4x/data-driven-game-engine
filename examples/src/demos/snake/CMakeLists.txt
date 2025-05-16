project(snake)

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE engine::engine)

find_package(SFML CONFIG REQUIRED COMPONENTS Graphics)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics)

add_subdirectory(window)

target_sources(${PROJECT_NAME} PRIVATE
        main.cpp
)
