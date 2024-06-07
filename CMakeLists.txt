cmake_minimum_required(VERSION 3.25)

project(engine C CXX)


add_library(${PROJECT_NAME})


option(engine_debug "Turn on debug mode for library" OFF)


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
            -pedantic-errors
            -Wconversion
            -Werror
            -Wno-missing-field-initializers
    )
endif ()


target_precompile_headers(${PROJECT_NAME} PRIVATE
        "$<$<COMPILE_LANGUAGE:CXX>:algorithm>"
        "$<$<COMPILE_LANGUAGE:CXX>:concepts>"
        "$<$<COMPILE_LANGUAGE:CXX>:expected>"
        "$<$<COMPILE_LANGUAGE:CXX>:fstream>"
        "$<$<COMPILE_LANGUAGE:CXX>:functional>"
        "$<$<COMPILE_LANGUAGE:CXX>:limits>"
        "$<$<COMPILE_LANGUAGE:CXX>:memory>"
        "$<$<COMPILE_LANGUAGE:CXX>:print>"
        "$<$<COMPILE_LANGUAGE:CXX>:ranges>"
        "$<$<COMPILE_LANGUAGE:CXX>:set>"
        "$<$<COMPILE_LANGUAGE:CXX>:span>"
        "$<$<COMPILE_LANGUAGE:CXX>:stdexcept>"
        "$<$<COMPILE_LANGUAGE:CXX>:string>"
        "$<$<COMPILE_LANGUAGE:CXX>:string_view>"
        "$<$<COMPILE_LANGUAGE:CXX>:thread>"
        "$<$<COMPILE_LANGUAGE:CXX>:type_traits>"
        "$<$<COMPILE_LANGUAGE:CXX>:unordered_map>"
        "$<$<COMPILE_LANGUAGE:CXX>:utility>"
        "$<$<COMPILE_LANGUAGE:CXX>:vector>"
        "$<$<COMPILE_LANGUAGE:CXX>:variant>"
)

include(dependencies.cmake)


target_include_directories(${PROJECT_NAME} PUBLIC src)

add_subdirectory(src)


unset(engine_debug CACHE)


###########
## TESTS ##
###########
add_subdirectory(example)
