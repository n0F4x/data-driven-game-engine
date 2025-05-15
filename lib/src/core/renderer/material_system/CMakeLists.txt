target_sources(${PROJECT_NAME} PRIVATE
        Program.cxx
        GraphicsPipelineBuilder.cxx
        Shader.cxx
        ShaderModule.cxx
        VertexLayout.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        material_system.cppm
        Program.ixx
        GraphicsPipelineBuilder.ixx
        Shader.ixx
        ShaderModule.ixx
        VertexLayout.ixx
)
