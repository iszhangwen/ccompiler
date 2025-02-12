cmake_minimum_required(VERSION 3.12)
project(ccompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
add_compile_options("-g")

add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(ssa)
add_subdirectory(x86)
add_subdirectory(drive)

add_subdirectory(test)