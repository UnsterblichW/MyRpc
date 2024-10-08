#include "MyRpcChannel.hpp"
#include "MyRpcInstance.hpp"
#include "user.pb.h"
#include <iostream>

int main(int argc, char **argv) {
	// 整个程序启动以后，想使用mprpc框架来享受rpc服务调用，一定需要先调用框架的初始化函数（只初始化一次）
	MyRpcInstance::Init(argc, argv);

	// 演示调用远程发布的rpc方法Login
	ExampleMsg::UserServiceRpc_Stub stub(new MyRpcChannel());
	// rpc方法的请求参数
	ExampleMsg::LoginRequest request;
	request.set_name("unsterblich");
	request.set_pwd("123456");
	// rpc方法的响应
	ExampleMsg::LoginResponse response;
	// 发起rpc方法的调用  同步的rpc调用过程  MprpcChannel::callmethod
	stub.Login(nullptr, &request, &response,
			   nullptr); // RpcChannel->RpcChannel::callMethod 集中来做所有rpc方法调用的参数序列化和网络发送

	// 一次rpc调用完成，读调用的结果
	if (0 == response.result().errcode()) {
		std::cout << "rpc login response success:" << response.sucess() << std::endl;
	} else {
		std::cout << "rpc login response error : " << response.result().errmsg() << std::endl;
	}

	// 演示调用远程发布的rpc方法Register
	ExampleMsg::RegisterRequest req;
	req.set_id(12345);
	req.set_name("myrpc");
	req.set_pwd("passwordmyrpc");
	ExampleMsg::RegisterResponse rsp;

	// 以同步的方式发起rpc调用请求，等待返回结果
	stub.Register(nullptr, &req, &rsp, nullptr);

	// 一次rpc调用完成，读调用的结果
	if (0 == rsp.result().errcode()) {
		std::cout << "rpc register response success:" << rsp.sucess() << std::endl;
	} else {
		std::cout << "rpc register response error : " << rsp.result().errmsg() << std::endl;
	}

	return 0;
}
