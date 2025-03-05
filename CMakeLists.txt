cmake_minimum_required(VERSION 3.12)
project(ccompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
add_compile_options("-g")

option(TOOL "set to switch to build tools..." OFF)
option(CCP "set to switch to build tools..." ON)

if (CCP) 
    message("start build ccompiler...")
    add_subdirectory(lexer)
    add_subdirectory(parser)
    add_subdirectory(ssa)
    add_subdirectory(x86)
    add_subdirectory(drive)
endif()

if (BUILD_TOOL)
    message("start build tools...")
    add_subdirectory(tools)
endif()

# add_subdirectory(test)