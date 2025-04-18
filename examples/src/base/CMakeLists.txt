add_library(base)
target_compile_features(base PRIVATE cxx_std_26)
target_compile_options(base PRIVATE
        -Wall
        -Wextra
        -pedantic-errors
        -Wconversion
        -Werror
        -Wno-missing-field-initializers
)
target_compile_definitions(base PRIVATE
        VULKAN_HPP_NO_TO_STRING
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
)
if (engine_debug)
    target_compile_definitions(base PRIVATE
            SPDLOG_ACTIVE_LEVEL=${spdlog_level}
    )
endif ()
target_sources(base PRIVATE
        DemoBase.cxx
        Controller.cxx
        init.cxx
        Renderer.cxx
)
target_sources(base PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
        Controller.ixx
        DemoBase.ixx
        init.ixx
        Renderer.ixx
)
target_link_libraries(base PUBLIC ${PROJECT_NAME})
