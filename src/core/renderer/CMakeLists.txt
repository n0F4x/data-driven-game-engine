add_subdirectory(base)
add_subdirectory(material_system)
add_subdirectory(model)

target_sources(${PROJECT_NAME} PRIVATE
        RenderScene.cpp
)
