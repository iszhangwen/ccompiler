add_subdirectory(frontend)
add_subdirectory(middleend)
add_subdirectory(backend)
add_subdirectory(support)

file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(cc ${SRC})
target_link_libraries(cc PRIVATE frontend backend)
