file(GLOB SRC "*")


add_executable(ccompiler ${SRC})
target_link_libraries(ccompiler parser codegen)