add_subdirectory(ktx2)
add_subdirectory(stb)

target_sources(${PROJECT_NAME} PRIVATE
        Image.cpp
)
