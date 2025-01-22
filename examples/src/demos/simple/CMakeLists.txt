add_executable(simple)
target_compile_features(simple PRIVATE cxx_std_26)
target_compile_options(simple PRIVATE
        -Wall
        -Wextra
        -pedantic-errors
        -Wconversion
        -Werror
        -Wno-missing-field-initializers
)
target_compile_definitions(simple PRIVATE
        VULKAN_HPP_NO_TO_STRING
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_SETTERS
        VULKAN_HPP_NO_SPACESHIP_OPERATOR
)
if (engine_debug)
    target_compile_definitions(simple PRIVATE
            SPDLOG_ACTIVE_LEVEL=${spdlog_level}
    )
endif ()
target_sources(simple PRIVATE
        src/main.cpp
)

fetchcontent_declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
        SYSTEM
)
fetchcontent_makeavailable(SFML)
target_link_libraries(simple PRIVATE SFML::Graphics)

target_link_libraries(simple PRIVATE ${PROJECT_NAME})
