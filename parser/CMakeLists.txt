file(GLOB SRC "*")

include_directories(${CMAKE_SOURCE_DIR}/lexer)
link_directories(${CMAKE_BINARY_DIR}/lexer)

add_library(parser ${SRC})
target_link_libraries(parser lexer)