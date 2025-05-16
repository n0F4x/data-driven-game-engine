project(simple)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET private_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${CURRENT_SOURCE_DIR}
        FILES
        Window.ixx
)

target_sources(${PROJECT_NAME} PRIVATE
        main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE engine_lib)
