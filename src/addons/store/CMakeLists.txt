target_sources(${PROJECT_NAME} PRIVATE
        Customization.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        store.cppm
        Customization.ixx
        Mixin.ixx
)
