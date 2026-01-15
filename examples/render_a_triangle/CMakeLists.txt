project(render_a_triangle)

add_executable(${PROJECT_NAME})

add_subdirectory(shaders)

target_sources(${PROJECT_NAME} PRIVATE
        main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE engine::engine)
