cmake_minimum_required(VERSION 3.25)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

project(engine C CXX)


add_library(${PROJECT_NAME})


option(engine_debug "Turn on debug mode for library" ON)


target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)


if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
            /W4
            /WX
    )
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Werror
            -Wno-missing-field-initializers
    )
endif ()


target_precompile_headers(${PROJECT_NAME} PRIVATE
        <algorithm>
        <concepts>
        <expected>
        <fstream>
        <functional>
        <iostream>
        <limits>
        <memory>
        <ranges>
        <set>
        <span>
        <sstream>
        <stdexcept>
        <string>
        <string_view>
        <thread>
        <type_traits>
        <unordered_map>
        <utility>
        <vector>
        <variant>
)

add_subdirectory(third_party)


target_include_directories(${PROJECT_NAME} PUBLIC src)

add_subdirectory(src)


unset(engine_debug CACHE)


###########
## TESTS ##
###########
add_subdirectory(example)
