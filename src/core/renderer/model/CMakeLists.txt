target_sources(${PROJECT_NAME} PRIVATE
        ModelLayout.cxx
        Requirements.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        Drawable.ixx
        ModelLayout.ixx
)
