cmake_minimum_required(VERSION 3.10)
project(cmap)

set(CMAKE_C_STANDARD_REQUIRED 99)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE MinSizeRel)
endif()

add_executable(cmap_example main.c src/map.c src/map.h)
add_library(cmap src/map.c)
target_include_directories(cmap PUBLIC src)
