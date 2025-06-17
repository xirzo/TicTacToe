add_library(network STATIC server.c client.c)

target_include_directories(network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(network PUBLIC math game_state)


