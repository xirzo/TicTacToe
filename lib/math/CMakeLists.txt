add_library(math STATIC vec2.c)

target_include_directories(math PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT MSVC)
    target_link_libraries(math PUBLIC m)
endif()
