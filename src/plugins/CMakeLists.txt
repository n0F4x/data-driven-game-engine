add_subdirectory(renderer)

target_sources(${PROJECT_NAME} PRIVATE
        Cache.cpp
        Renderer.cpp
        Window.cpp
)
