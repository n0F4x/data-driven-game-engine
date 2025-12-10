target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        extensions.cppm
        AddonsPlugin.ixx
        FunctionalPlugin.ixx
        MetaInfo.ixx
        MetaInfoAddon.ixx
        MetaInfoPlugin.ixx
        RunnablePlugin.ixx
)
