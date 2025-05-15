target_sources(${PROJECT_NAME} PRIVATE
        Store.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        store.cppm
        storable_c.ixx
        Store.ixx
)
