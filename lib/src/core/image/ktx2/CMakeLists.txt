target_sources(${PROJECT_NAME} PRIVATE
        Image.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        ktx2.cppm
        Image.ixx
        MimeType.ixx
)
