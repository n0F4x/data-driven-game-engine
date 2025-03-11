add_library(${PROJECT_NAME})

set(ENGINE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


option(engine_debug "Turn on debug mode for library" OFF)


target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_26)

# We only support Clang of now
# TODO: specify these settings outside of CMake
target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -pedantic-errors
        -Wconversion
        -Werror
        -Wno-missing-field-initializers
        -fvisibility=hidden
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        CHECK_LIFETIME_BOUND
)

if (engine_debug)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            ENGINE_LOG_LEVEL=ENGINE_LOG_LEVEL_DEBUG
            ENGINE_ENABLE_STATIC_TESTS # TODO: migrate rest of static tests to tests executable
    )
endif ()


include(cmake/dependencies.cmake)


add_subdirectory(src)


unset(engine_debug CACHE)
