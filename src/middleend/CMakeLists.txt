add_library(middleend_interface INTERFACE)
target_include_directories(middleend_interface INTERFACE
    ir
    optimizer
)

file(GLOB_RECURSE SRC "*.cpp" "*.cc" "*.c")
add_library(middleend ${SRC})
target_link_libraries(middleend PUBLIC middleend_interface)
target_link_libraries(middleend PRIVATE support frontend)

