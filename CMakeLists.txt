cmake_minimum_required (VERSION 3.21)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


project (engine)


add_subdirectory(vendor/gsl)
add_subdirectory(vendor/entt)
add_subdirectory(vendor/glfw)

find_package(Vulkan REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SRC src/*.cpp)


add_executable(${PROJECT_NAME} ${SRC})

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC GSL)
target_link_libraries(${PROJECT_NAME} PUBLIC EnTT)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
