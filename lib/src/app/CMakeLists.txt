target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        app.cppm
        addon_c.ixx
        App.ixx
        app_c.ixx
        Builder.ixx
        builder_c.ixx
        create.ixx
        decays_to_addon_c.ixx
        decays_to_app_c.ixx
        decays_to_builder_c.ixx
        decays_to_plugin_c.ixx
        has_addons_c.ixx
        has_plugins_c.ixx
        plugin_c.ixx
)
