target_sources(${PROJECT_NAME} PUBLIC
        FILE_SET public_cxx_modules
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES
        measurement.cppm
        frames_per_second.ixx
        refresh_rate.ixx
        tick_rate.ixx
        updates_per_second.ixx
)
