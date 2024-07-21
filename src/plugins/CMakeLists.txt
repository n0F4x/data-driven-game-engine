add_subdirectory(renderer)

target_sources(${PROJECT_NAME} PRIVATE
        Renderer.cpp
        Window.cpp
)
