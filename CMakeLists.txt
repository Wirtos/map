cmake_minimum_required(VERSION 3.10)
project(cmap LANGUAGES C VERSION 2.2.2)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(cmap src/cmap.c)
target_include_directories(cmap PUBLIC src)

add_executable(cmap_example EXCLUDE_FROM_ALL main.c)
target_link_libraries(cmap_example PRIVATE cmap)
