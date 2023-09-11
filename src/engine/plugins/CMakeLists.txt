add_subdirectory(renderer)
add_subdirectory(window)

target_sources(${PROJECT_NAME} PUBLIC
        Renderer.cpp
        Window.cpp
        )

