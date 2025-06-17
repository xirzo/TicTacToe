add_library(game_state STATIC game_state.c)

target_include_directories(game_state PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(game_state PUBLIC global network)
