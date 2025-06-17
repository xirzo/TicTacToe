add_library(button STATIC button.c)

target_link_libraries(button PUBLIC math raylib)


target_include_directories(button PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
