cmake_minimum_required(VERSION 3.10)
project(cmap)

set(CMAKE_C_STANDARD 99 REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE MinSizeRel)
endif()

add_executable(cmap main.c src/map.c src/map.h)