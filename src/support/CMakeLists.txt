add_library(support_interface INTERFACE)
target_include_directories(support_interface INTERFACE 
    ccerror
)

file(GLOB_RECURSE SRC "*.cpp" "*.cc")
add_library(support ${SRC})
# 关键：将 PRIVATE 改为 PUBLIC
target_link_libraries(support PUBLIC support_interface)
