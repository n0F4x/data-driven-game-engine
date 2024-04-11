add_subdirectory(renderer)

target_sources(${PROJECT_NAME} PRIVATE
        Cache.cpp
        Logger.cpp
        Renderer.cpp
        Window.cpp
)
