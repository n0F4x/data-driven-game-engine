target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        messages.cppm
        Mailbox.ixx
        Provider.ixx
        Receiver.ixx
        Sender.ixx
)
