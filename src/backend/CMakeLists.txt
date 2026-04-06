add_library(backend_interface INTERFACE)
target_include_directories(backend_interface INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/target
    ${CMAKE_CURRENT_SOURCE_DIR}/mir
    ${CMAKE_CURRENT_SOURCE_DIR}/instSelect
    ${CMAKE_CURRENT_SOURCE_DIR}/instSchedule
    ${CMAKE_CURRENT_SOURCE_DIR}/regAlloca
)

file(GLOB_RECURSE SRC "*.cpp" "*.cc" "*.c")
add_library(backend ${SRC})
target_link_libraries(backend PUBLIC backend_interface)
target_link_libraries(backend PUBLIC frontend middleend support)
target_include_directories(backend PUBLIC
    ${CMAKE_SOURCE_DIR}/src/frontend/ast
)