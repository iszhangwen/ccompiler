file(GLOB SRC "*")
file(GLOB ex_src "preprocessor.*")
list(REMOVE_ITEM SRC ${ex_src})
message(${SRC})

add_library(lexer ${SRC})
target_link_libraries(lexer base)

target_include_directories(lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})