cmake_minimum_required(VERSION 3.22)
project(raycast_3d)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.c??)
file(GLOB_RECURSE HEADER_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h)

include_directories(src)

add_executable(raycast_3d ${SRC_FILES} ${HEADER_FILES})
target_link_libraries(raycast_3d ${SDL2_LIBRARIES})
