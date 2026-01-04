# 构建测试框架库
add_subdirectory(doctest)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/doctest/doctest)

# 收集所有 .c 源文件（排除特定文件）
file(GLOB source_files "*.cpp")

# 为每个源文件生成一个可执行文件
foreach(src ${source_files})
    # 获取不带扩展名的文件名（如 foo.c → foo）
    get_filename_component(exec_name ${src} NAME_WE)
    
    # 生成可执行文件
    add_executable(${exec_name} ${src})
    
    # 可选：统一设置编译选项
    target_compile_options(${exec_name} PRIVATE -Wall -Wextra)
    
    # 可选：链接公共库
    target_link_libraries(${exec_name} PRIVATE doctest::doctest)

    # 注册测试
    add_test(NAME ${exec_name} COMMAND ${exec_name})
endforeach()