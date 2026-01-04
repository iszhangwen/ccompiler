add_library(backend_interface INTERFACE)
target_include_directories(backend_interface INTERFACE
    codegen
    target
)

file(GLOB_RECURSE SRC "*.cpp" "*.cc" "*.c")
add_library(backend ${SRC})
target_link_libraries(backend PUBLIC backend_interface)
target_link_libraries(backend PUBLIC frontend support)