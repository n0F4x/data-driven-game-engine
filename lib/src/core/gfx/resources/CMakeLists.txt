target_sources(${PROJECT_NAME} PRIVATE
        Image.cxx
        image_helpers.cxx
        virtual_image_helpers.cxx
        VirtualImage.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        resources.cppm
        Image.ixx
        image_helpers.ixx
        virtual_image_helpers.ixx
        VirtualImage.ixx
)
