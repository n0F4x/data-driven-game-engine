target_sources(${PROJECT_NAME} PRIVATE
        Program.cpp
        GraphicsPipelineBuilder.cpp
        Shader.cpp
        ShaderModule.cpp
        VertexLayout.cpp
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        Program.ixx
        GraphicsPipelineBuilder.ixx
        Shader.ixx
        ShaderModule.ixx
        VertexLayout.ixx
)
