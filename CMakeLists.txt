cmake_minimum_required(VERSION 3.10)

foreach(vspec IN ITEMS MAJOR MINOR PATCH)
    file(STRINGS src/cmap.h vspec REGEX "#define MAP_VER_${vspec} ([0-9]+)")
    string(SUBSTRING "${vspec}" 22 -1 vspec)
    list(APPEND CMAP_VER ${vspec})
endforeach()
unset(vspec)
string(JOIN "." CMAP_VER ${CMAP_VER})

project(cmap LANGUAGES C VERSION ${CMAP_VER})

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(cmap src/cmap.c)
target_include_directories(cmap PUBLIC src)

add_executable(cmap_example EXCLUDE_FROM_ALL main.c)
target_link_libraries(cmap_example PRIVATE cmap)

add_executable(cmap_test EXCLUDE_FROM_ALL test/test.c)
target_link_libraries(cmap_test PRIVATE cmap)
