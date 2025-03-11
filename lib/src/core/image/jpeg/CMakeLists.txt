target_sources(${PROJECT_NAME} PRIVATE
        Image.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        jpeg.cppm
        Image.ixx
        MimeType.ixx
)
