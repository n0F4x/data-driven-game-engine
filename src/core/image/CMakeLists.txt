add_subdirectory(jpeg)
add_subdirectory(ktx2)
add_subdirectory(png)
add_subdirectory(stb)

target_sources(${PROJECT_NAME} PRIVATE
        Image.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        Image.ixx
)
