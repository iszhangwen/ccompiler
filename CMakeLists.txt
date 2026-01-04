cmake_minimum_required(VERSION 3.12)
project(ccompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
add_compile_options("-g")

option(CCP "set to switch to build SRC..." ON)
option(TOOL "set to switch to build tools..." OFF)

if (CCP) 
    message("start build ccompiler...")
    add_subdirectory(src)
endif()

if (TOOL)
    message("start build tools...")
    add_subdirectory(tools)
endif()
