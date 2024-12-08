cmake_minimum_required(VERSION 3.5.0)

project(
  TicTacToe
  VERSION 1.0
  DESCRIPTION "C++ Year 2024 Labwork2"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -O3 -ftrapv -fsanitize=address,undefined")

add_subdirectory(lib)
add_subdirectory(bin)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
