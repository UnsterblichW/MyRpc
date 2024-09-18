
# GPT小贴士：CMake官方不建议用aux_source_directory命令，aux_source_directory虽然会找出所有源文件（.cpp .c .cxx .c++ .C）
# 但是可读性较差，而且仅仅支持搜索当前CMakeLists.txt所在一级目录下的源文件
# aux_source_directory(. CPP_SOURCES) 

# file(GLOB ...) 可以找当前目录，也可以递归寻找当前目录下的子目录
#file(GLOB CPP_SOURCES "*.cpp" "*.cc" "*.c")
#message("CPP_SOURCES is : " ${CPP_SOURCES})

# 还是直接写明所需的每个源文件比较清晰
set(CPP_SOURCES 
        MyRpcChannel.cpp
        MyRpcConfigMgr.cpp
        MyRpcController.cpp
        MyRpcInstance.cpp
        MyRpcProvider.cpp
        MyRpcZKClientWapper.cpp
        rpcheader.pb.cc)

add_library(myrpc ${CPP_SOURCES})

target_link_libraries(myrpc muduo_net muduo_base pthread zookeeper_mt)





