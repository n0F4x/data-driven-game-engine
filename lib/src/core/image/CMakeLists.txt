add_subdirectory(jpeg)
add_subdirectory(ktx2)
add_subdirectory(png)

target_sources(${PROJECT_NAME} PRIVATE
        Image.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        image.cppm
        Image.ixx
)
