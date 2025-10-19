cmake_minimum_required(VERSION 3.25.0)

project(tic_tac_toe)

option(FETCH_LIBS OFF)

add_compile_options(-Wunused-function -Wall -Wextra)

if (FETCH_LIBS)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG master
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(raylib)
else()
    find_package(raylib REQUIRED)
endif(FETCH_LIBS)
unset(FETCH_LIBS CACHE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(bin)
add_subdirectory(lib)

file(COPY ${CMAKE_SOURCE_DIR}/start.sh DESTINATION ${CMAKE_BINARY_DIR}/bin)
