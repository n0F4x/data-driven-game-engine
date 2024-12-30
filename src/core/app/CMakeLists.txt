target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        app.cppm
        App.ixx
        App-details.ixx
        App-fwd.ixx
        App-mixin_c.ixx
        Builder.ixx
        Builder-customization_c.ixx
        Builder-details.ixx
        Builder-fwd.ixx
)
