target_sources(${PROJECT_NAME} PRIVATE
        Builder.cxx
        Scene.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        scene.cppm
        Scene.ixx
)
