message("start compiler x86...")

file(GLOB SRC "*")

add_library(codegen ${SRC})

target_include_directories(codegen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})