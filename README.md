# 一个简单的RPC框架

## 项目概述
1. 使用[`ProtoBuf`](https://github.com/protocolbuffers/protobuf)来实现网络通信的序列化，使用[`ZooKeeper`](https://github.com/apache/zookeeper)来做服务发现，支持RPC服务端分布式部署
2. 使用[`muduo`](https://github.com/UnsterblichW/muduo)（使用了C++17支持的一些新功能）来作为本RPC框架的网络通信模块
3. 使用了[`MyAsyncLog`](https://github.com/UnsterblichW/MyAsyncLog)作为本RPC框架的日志输出模块
------------------------------
## 安装流程
提醒：
    本项目测试于**Ubuntu22.04**环境，所需的C++版本为**C++11（最好是C++17）**及以上，使用CMake构建项目，CMake版本需要在3.11及以上
1. 需要预先安装的第三方依赖
   1. 安装 [`ProtoBuf`](https://github.com/protocolbuffers/protobuf) ，流程参考 [官方文档](https://github.com/protocolbuffers/protobuf/blob/main/src/README.md)
   2. 安装 [`ZooKeeper`](https://github.com/apache/zookeeper)，注意，将会需要用到官方提供的 [C语言客户端](https://github.com/apache/zookeeper/blob/master/zookeeper-client/zookeeper-client-c)
   3. 请将`ProtoBuf`和`ZooKeeper`都安装至系统目录，即确保可以用 `#include "<>"` 形式找到所需的头文件
2. 安装好上述前置依赖后，需要在本项目根目录下的`CMakeLists.txt`中将以下两行muduo库的所在位置修改
   ```MakeFile
        # 添加头文件路径（这里是muduo库所在的头文件目录）
        include_directories("/home/unsterblich/muduo/build/release-cpp17/release-install-cpp17/include")
        # 添加库文件路径（这里是muduo库所在的库文件目录）
        link_directories("/home/unsterblich/muduo/build/release-cpp17/release-install-cpp17/lib")
   ``` 
3. 在本项目根目录下创建build目录，使用CMake构建项目，使用Make编译
   ```shell
   mkdir build
   cd build
   cmake ..
   make -j4 #可以自行将并行编译的线程数调高一些
   ``` 
------------------------------
## 本RPC使用流程
1. 以example文件夹中的代码为例，需要利用protobuf的语法来定义需要序列化的消息格式，例如`user.proto`和`friend.proto`
2. 用`protoc`命令通过`*.proto`文件生成业务逻辑所需的`*.pb.h`和`*.pb.cc`文件
3. 在业务逻辑中重写函数，类似于`example/remote_process/UserService.cpp`之中对于`UserService`的实现
------------------------------
## 项目架构概述
本项目的架构图如下：
![image](Workflow.png)

简述运行流程：
1. MyRpc的服务端先启动，将会把自己支持的服务和方法注册到zookeeper里面去，服务注册的znode节点都是临时节点，zookeeper封装的客户端提供了心跳探测机制来确认这些rpc服务端是否存活；
2. MyRpc的客户端后启动，先向zookeeper询问想要调用的远程方法的ip和端口；
3. MyRpc的客户端向服务端发送的消息格式也是通过protobuf来定义的，可见`src/rpcheader.proto`，客户端按这个格式组装将要调用的远程方法的服务名、方法名、参数表，通过protobuf序列化成二进制字符串，然后通过muduo框架发送给MyRpc服务端；
4. Muduo为MyRpc客户端和服务端之间的数据传输提供了保障，MyRpc服务端在收到二进制字符串后，先通过protobuf反序化得到客户端所需的服务名、方法名、参数表，执行对于的逻辑后，将处理得到的结果重新通过protobuf序列化成二进制字符串，发回给MyRpc客户端；
5. MyRpc客户端收到服务端返回的表示远程调用结果的二进制字符串，用protobuf反序列化得到所需的数据。
