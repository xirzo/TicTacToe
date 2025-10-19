add_executable(${PROJECT_NAME} client.c)

target_link_libraries(${PROJECT_NAME} PRIVATE input math network game_state raylib button global)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/assets/)

add_executable(tic_tac_toe_server server.c)

target_link_libraries(tic_tac_toe_server PRIVATE input math network game_state raylib button global)
target_include_directories(tic_tac_toe_server PRIVATE ${CMAKE_SOURCE_DIR}/lib/)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUGGING>)
