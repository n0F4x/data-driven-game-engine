target_sources(${PROJECT_NAME} PRIVATE
        Buffer.cpp
        Image.cpp
)

target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES
        Buffer.ixx
        Image.ixx
        RandomAccessBuffer.ixx
        SeqWriteBuffer.ixx
)
