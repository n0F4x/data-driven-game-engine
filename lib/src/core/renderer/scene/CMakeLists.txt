target_sources(${PROJECT_NAME} PRIVATE
        Builder.cxx
        Scene.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        scene.cppm
        Scene.ixx
)
