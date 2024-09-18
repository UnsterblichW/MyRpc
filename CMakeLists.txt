cmake_minimum_required(VERSION 3.11)

project(myrpc)

# 生成debug版本，可以进行gdb调试
set(CMAKE_BUILD_TYPE "Debug")

# 设置项目可执行文件输出的路径
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 设置项目库文件输出的路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 添加头文件路径（这里是muduo库所在的头文件目录）
include_directories("/home/unsterblich/muduo/build/release-cpp17/release-install-cpp17/include")
# 添加库文件路径（这里是muduo库所在的库文件目录）
link_directories("/home/unsterblich/muduo/build/release-cpp17/release-install-cpp17/lib")

# 设置项目编译头文件搜索路径 -I
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)

# 设置项目库文件搜索路径 -L
link_directories(${PROJECT_SOURCE_DIR}/lib)

# src包含了mprpc框架所有的相关代码
add_subdirectory(src)
# example包含了mprpc框架使用的示例代码
add_subdirectory(example)





