add_subdirectory(containers)
add_subdirectory(memory)
add_subdirectory(meta)

target_sources(${PROJECT_NAME} PRIVATE
        Bool.cpp
        Overloaded.cpp
)
