target_sources(${PROJECT_NAME} PRIVATE
        Buffer.cxx
        Image.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        resources.cppm
        Buffer.ixx
        Image.ixx
        RandomAccessBuffer.ixx
        SeqWriteBuffer.ixx
)
