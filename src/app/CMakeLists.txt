target_sources(${PROJECT_NAME} PRIVATE
        App.cpp
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        App.ixx
)
