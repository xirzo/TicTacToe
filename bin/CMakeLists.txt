add_executable(${PROJECT_NAME} client.c)

find_package(logger REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE logger::logger)

target_link_libraries(${PROJECT_NAME} PRIVATE input math network game_state raylib button global)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/)

add_executable(tic_tac_toe_server server.c)

target_link_libraries(tic_tac_toe_server PRIVATE input math network game_state raylib button global)
target_include_directories(tic_tac_toe_server PRIVATE ${CMAKE_SOURCE_DIR}/lib/)


add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E echo "Force copying assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "${CMAKE_CURRENT_BINARY_DIR}/"

    COMMENT "Force copying assets"
    VERBATIM
)

add_dependencies(${PROJECT_NAME} copy_assets)

set_target_properties(copy_assets PROPERTIES
    EXCLUDE_FROM_ALL FALSE
)
