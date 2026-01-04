add_library(frontend_interface INTERFACE)
target_include_directories(frontend_interface INTERFACE
    lexer
    ast
    parser
    semantic
)

file(GLOB_RECURSE SRC "*.cpp" "*.cc" "*.c")
add_library(frontend ${SRC})
target_link_libraries(frontend PUBLIC frontend_interface)
target_link_libraries(frontend PRIVATE support)

