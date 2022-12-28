cmake_minimum_required (VERSION 3.21)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project ("engine")

file(GLOB SRC CONFIGURE_DEPENDS "src/**.cpp")

include_directories("include")

set(SFML_DIR lib/cmake/SFML)

find_package(SFML 2.5.1 COMPONENTS graphics audio system window REQUIRED)

add_executable (${PROJECT_NAME} ${SRC})

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC sfml-graphics sfml-audio sfml-system sfml-window)
