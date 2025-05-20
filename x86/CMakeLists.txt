message("start compiler x86...")

file(GLOB SRC "*")

include_directories(${CMAKE_SOURCE_DIR}/parser)

add_library(codegen ${SRC})