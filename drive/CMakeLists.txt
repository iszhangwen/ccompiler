file(GLOB SRC "*")

include_directories(${CMAKE_SOURCE_DIR}/lexer)
link_directories(${CMAKE_BINARY_DIR}/lexer)

include_directories(${CMAKE_SOURCE_DIR}/parser)
link_directories(${CMAKE_BINARY_DIR}/parser)

add_executable(ccompiler ${SRC})
target_link_libraries(ccompiler lexer parser)