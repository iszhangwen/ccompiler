message("start compiler sema....")
file(GLOB SRC "*")

add_library(sema ${SRC})
target_link_libraries(sema parser)

target_include_directories(sema PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
