target_sources(${PROJECT_NAME} PRIVATE
        Buffer.cxx
        Image.cxx
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${ENGINE_SRC_DIR}
        FILES
        resources.cppm
        Buffer.ixx
        Image.ixx
        RandomAccessBuffer.ixx
        SeqWriteBuffer.ixx
)
