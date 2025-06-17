add_library(global INTERFACE properties.h types.h)
target_include_directories(global INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
