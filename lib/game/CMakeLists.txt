add_library(game game.cpp game.h)

target_link_libraries(game PRIVATE board)

target_include_directories(game PUBLIC ../board/)
