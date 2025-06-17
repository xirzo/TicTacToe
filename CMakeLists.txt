cmake_minimum_required(VERSION 3.5.0)

project(tic_tac_toe)

find_package(raylib REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(bin)
add_subdirectory(lib)
