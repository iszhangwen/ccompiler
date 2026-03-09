add_subdirectory(support)
add_subdirectory(frontend)
add_subdirectory(middleend)
add_subdirectory(backend)

file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(cc ${SRC})
target_link_libraries(cc PRIVATE frontend middleend backend)
