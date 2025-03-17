cmake_minimum_required(VERSION 3.30)

project(engine C CXX)


option(engine_debug "Turn on debug mode for library" OFF)
option(engine_test "Turn on unit tests for library" OFF)
option(engine_examples "Turn on examples for library" OFF)


add_subdirectory(lib)

if (engine_test)
    add_subdirectory(tests)
endif ()

if (engine_examples)
    add_subdirectory(examples)
endif ()


unset(engine_debug CACHE)
unset(engine_test CACHE)
unset(engine_examples CACHE)
