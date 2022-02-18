cmake_minimum_required(VERSION 3.22)
project(raycast_3d)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(raycast_3d main.cpp)
target_link_libraries(raycast_3d ${SDL2_LIBRARIES})
