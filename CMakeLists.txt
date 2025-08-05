cmake_minimum_required(VERSION 3.12)
project(ccompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
add_compile_options("-g")

option(TOOL "set to switch to build tools..." OFF)
option(CCP "set to switch to build SRC..." ON)

if (CCP) 
    message("start build ccompiler...")
    add_subdirectory(base)
    add_subdirectory(lexer)
    add_subdirectory(parser)
    add_subdirectory(sema)
    add_subdirectory(ssa)
    add_subdirectory(codegen)
    add_subdirectory(drive)
endif()

if (TOOL)
    message("start build tools...")
    add_subdirectory(tools)
endif()

enable_testing()
add_subdirectory(test)