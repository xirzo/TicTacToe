add_library(input input_handler.cpp input_handler.h)

target_link_libraries(input PRIVATE game)

target_include_directories(game PUBLIC ../game/)
