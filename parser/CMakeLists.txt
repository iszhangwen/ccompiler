file(GLOB_RECURSE SRC "*")

add_library(parser ${SRC})
target_link_libraries(parser lexer sema)
target_include_directories(parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(parser_interface INTERFACE)
target_include_directories(parser_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})