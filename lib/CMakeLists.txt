cmake_minimum_required(VERSION 3.30)

project(engine VERSION 0.0.1)

set(${project_prefix}LOG_LEVEL ${project_prefix}LOG_LEVEL_INFO CACHE STRING "Set log level")
set_property(CACHE ${project_prefix}LOG_LEVEL PROPERTY STRINGS
        ${project_prefix}LOG_LEVEL_OFF
        ${project_prefix}LOG_LEVEL_CRITICAL
        ${project_prefix}LOG_LEVEL_ERROR
        ${project_prefix}LOG_LEVEL_WARNING
        ${project_prefix}LOG_LEVEL_INFO
        ${project_prefix}LOG_LEVEL_DEBUG
        ${project_prefix}LOG_LEVEL_TRACE
)


add_library(${PROJECT_NAME})
add_library(engine::engine ALIAS ${PROJECT_NAME})


target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_26)

include(cmake/set_log_level.cmake)
set_log_level(${PROJECT_NAME} ${project_prefix}LOG_LEVEL)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        ${project_prefix}VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        ${project_prefix}VERSION_MINOR=${PROJECT_VERSION_MINOR}
        ${project_prefix}VERSION_PATCH=${PROJECT_VERSION_PATCH}
        CHECK_LIFETIME_BOUND
)

if (${project_prefix}ENABLE_TESTS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            ${project_prefix}ENABLE_UNIT_TESTS
    )
endif ()


include(cmake/dependencies.cmake)


add_subdirectory(src)


install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        FILE_SET public_headers FILE_SET public_cxx_modules
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
