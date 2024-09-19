/*
 * author : https://github.com/UnsterblichW
 * brief  : rpc 服务的实例类
 */

#include "MyRpcProvider.hpp"
#include "MyRpcInstance.hpp"
#include "MyRpcZKClientWapper.hpp"
#include "rpcheader.pb.h"
#include <functional>
#include <iostream>
#include <string>
#include <unistd.h>
#include <malog/logger.h>
using namespace malog;

void RpcProvider::NotifyService(google::protobuf::Service *service) {
	ServiceInfo service_info;

	// 获取了服务对象的描述信息
	google::protobuf::ServiceDescriptor const *pserviceDesc = service->GetDescriptor();
	// 获取服务的名字
	std::string service_name = pserviceDesc->name();
	// 获取服务对象service的方法的数量
	int methodCnt = pserviceDesc->method_count();

	// std::cout << "service_name:" << service_name << std::endl;
	Log::trace("service_name: {}", service_name);

	for (int i = 0; i < methodCnt; ++i) {
		// 获取了服务对象指定下标的服务方法的描述（抽象描述） UserService   Login
		google::protobuf::MethodDescriptor const *pmethodDesc = pserviceDesc->method(i);
		std::string method_name = pmethodDesc->name();
		service_info.m_methodMap.insert({method_name, pmethodDesc});

		// std::cout << "method_name: " << method_name.c_str() << std::endl;
		Log::trace("method_name: {}", method_name);
	}
	service_info.m_service = service;
	m_serviceMap.insert({service_name, service_info});
}

void RpcProvider::Run() {
	// 读取配置文件rpcserver的信息
	std::string ip = std::string(MyRpcInstance::getInstance().getConfig().getRpcServerIp());
	uint16_t port = MyRpcInstance::getInstance().getConfig().getRpcServerPort();
	muduo::net::InetAddress address(ip, port);

	// 创建TcpServer对象
	muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");

	// 绑定连接回调和消息读写回调方法  分离了网络代码和业务代码
	server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
	server.setMessageCallback(
		std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	// 设置muduo库的线程数量
	server.setThreadNum(4);

	// 把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
	// session timeout   30s     zkclient 网络I/O线程  1/3 * timeout 时间发送ping消息
	ZKClient zkCli;
	zkCli.Start();
	// service_name为永久性节点    method_name为临时性节点
	for (auto &sp: m_serviceMap) {
		// /service_name   /UserServiceRpc
		std::string service_path = "/" + sp.first;
		zkCli.Create(service_path.c_str(), nullptr, 0);
		for (auto &mp: sp.second.m_methodMap) {
			// /service_name/method_name   /UserServiceRpc/Login 存储当前这个rpc服务节点主机的ip和port
			std::string method_path = service_path + "/" + mp.first;
			char method_path_data[128] = {0};
			sprintf(method_path_data, "%s:%d", ip.c_str(), port);
			// ZOO_EPHEMERAL表示znode是一个临时性节点
			zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
		}
	}

	// rpc服务端准备启动，打印信息
	//std::cout << "RpcProvider start service at ip:" << ip << " port:" << port << std::endl;
	Log::trace("RpcProvider start service at ip: {} port: {}", ip, port);

	// 启动muduo的网络服务
	server.start();
	m_eventLoop.loop();
}

void RpcProvider::OnConnection(muduo::net::TcpConnectionPtr const &conn) {
	if (!conn->connected()) {
		// 和rpc client的连接断开了
		conn->shutdown();
	}
}

void RpcProvider::OnMessage(muduo::net::TcpConnectionPtr const &conn, muduo::net::Buffer *buffer, muduo::Timestamp) {
	// 网络上接收的远程rpc调用请求的字符流
	std::string recv_buf = buffer->retrieveAllAsString();

	// 数据头格式: header_size(4个字节) + header_str + args_str
	uint32_t header_size = 0;
	recv_buf.copy((char *)&header_size, 4, 0);

	// 根据header_size读取数据头的原始字符流，反序列化数据，得到rpc请求的详细信息
	std::string rpc_header_str = recv_buf.substr(4, header_size);
	myrpc::RpcHeader rpcHeader;
	std::string service_name;
	std::string method_name;
	uint32_t args_size;
	if (rpcHeader.ParseFromString(rpc_header_str)) {
		// 数据头反序列化成功
		service_name = rpcHeader.service_name();
		method_name = rpcHeader.method_name();
		args_size = rpcHeader.args_size();
	} else {
		// 数据头反序列化失败
		//std::cout << "rpc_header_str:" << rpc_header_str << " parse error!" << std::endl;
		Log::trace("rpc_header_str: {} parse error!", rpc_header_str);
		return;
	}

	// 获取rpc方法参数的字符流数据
	std::string args_str = recv_buf.substr(4 + header_size, args_size);

	// 打印调试信息
	// std::cout << "============================================" << std::endl;
	// std::cout << "header_size: " << header_size << std::endl;
	// std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
	// std::cout << "service_name: " << service_name << std::endl;
	// std::cout << "method_name: " << method_name << std::endl;
	// std::cout << "args_str: " << args_str << std::endl;
	// std::cout << "============================================" << std::endl;

	Log::trace("============================================");
	Log::trace("header_size: {}", header_size);
	Log::trace("rpc_header_str: {}", rpc_header_str);
	Log::trace("service_name: {}", service_name);
	Log::trace("method_name: {}", method_name);
	Log::trace("args_str: {}", args_str);
	Log::trace("============================================");

	// 获取service对象和method对象
	auto service_iter = m_serviceMap.find(service_name);
	if (service_iter == m_serviceMap.end()) {
		//std::cout << service_name << " is not exist!" << std::endl;
		Log::error("{} is not exist!", service_name);
		return;
	}

	auto method_iter = service_iter->second.m_methodMap.find(method_name);
	if (method_iter == service_iter->second.m_methodMap.end()) {
		//std::cout << service_name << ":" << method_name << " is not exist!" << std::endl;
		Log::error("{} : {} is not exist!", service_name, method_name);
		return;
	}

	google::protobuf::Service *service = service_iter->second.m_service;	// 获取service对象
	google::protobuf::MethodDescriptor const *method = method_iter->second; // 获取method对象

	// 生成rpc方法调用的请求request和响应response参数
	google::protobuf::Message *request = service->GetRequestPrototype(method).New();
	if (!request->ParseFromString(args_str)) {
		//std::cout << "request parse error, content:" << args_str << std::endl;
		Log::error("request parse error, content: {}", args_str);
		return;
	}
	google::protobuf::Message *response = service->GetResponsePrototype(method).New();

	// 给下面的method方法的调用，绑定一个Closure的回调函数
	google::protobuf::Closure *done =
		google::protobuf::NewCallback<RpcProvider, muduo::net::TcpConnectionPtr const &, google::protobuf::Message *>(
			this, &RpcProvider::SendRpcResponse, conn, response);

	// 在框架上根据远端rpc请求，调用当前rpc节点上发布的方法
	service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::SendRpcResponse(muduo::net::TcpConnectionPtr const &conn, google::protobuf::Message *response) {
	std::string response_str;
	if (response->SerializeToString(&response_str)) // response进行序列化
	{
		// 序列化成功后，通过网络把rpc方法执行的结果发送会rpc的调用方
		conn->send(response_str);
	} else {
		//std::cout << "serialize response_str error!" << std::endl;
		Log::error("serialize response_str error!");
	}
	conn->shutdown(); // 模拟http的短链接服务，由rpcprovider主动断开连接
}
