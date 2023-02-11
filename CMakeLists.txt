cmake_minimum_required (VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


project(engine)


add_library(${PROJECT_NAME} INTERFACE)


if(MSVC)
  target_compile_options(${PROJECT_NAME} INTERFACE
    /W4
    /WX
  )
else()
  target_compile_options(${PROJECT_NAME} INTERFACE
    -Wall
    -Wextra
    -Werror
    -fconcepts-diagnostics-depth=3
)
endif()

target_include_directories(${PROJECT_NAME} INTERFACE
  src
)

add_subdirectory(src/app)
add_subdirectory(src/engine)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} INTERFACE
  Microsoft.GSL::GSL
  Vulkan::Vulkan
  glm::glm
  glfw
  EnTT::EnTT
)


add_executable(test examples/src/main.cpp)
target_link_libraries(test ${PROJECT_NAME})
