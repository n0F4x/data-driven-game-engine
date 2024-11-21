target_sources(${PROJECT_NAME} PRIVATE
        Image.cpp
        image_helpers.cpp
        virtual_image_helpers.cpp
        VirtualImage.cpp
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        Image.ixx
        image_helpers.ixx
        virtual_image_helpers.ixx
        VirtualImage.ixx
)
