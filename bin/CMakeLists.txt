add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ../lib/)

target_link_libraries(${PROJECT_NAME} PRIVATE game)
target_link_libraries(${PROJECT_NAME} PRIVATE input)
