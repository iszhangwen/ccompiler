file(GLOB SRC "*")

include_directories(${CMAKE_SOURCE_DIR}/lexer)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
link_directories(${CMAKE_BINARY_DIR}/lexer)

add_library(parser ${SRC})
target_link_libraries(parser lexer)