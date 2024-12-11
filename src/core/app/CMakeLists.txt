target_sources(${PROJECT_NAME} PRIVATE
        App.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ../../..
        FILES
        app.cppm
        App.ixx
)
