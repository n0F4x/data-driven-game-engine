target_sources(${PROJECT_NAME} PRIVATE
        Builder.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        app.cppm
        App.ixx
        Builder.ixx
)
