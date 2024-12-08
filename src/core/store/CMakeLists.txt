target_sources(${PROJECT_NAME} PRIVATE
        Store.cxx
        StoreView.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        Store.ixx
        StoreView.ixx
)
