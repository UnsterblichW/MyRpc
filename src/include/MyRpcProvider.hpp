/*
 * author : https://github.com/UnsterblichW
 * brief  : rpc 服务的具体提供类
 */

#ifndef MYPRC_RPCPROVIDER_H
#define MYPRC_RPCPROVIDER_H

#include "google/protobuf/service.h"
#include <google/protobuf/descriptor.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>
#include <string>

class RpcProvider {
public:
	// 这里是框架提供给外部使用的，可以发布rpc方法的函数接口
	void NotifyService(google::protobuf::Service *service);
	// 启动rpc服务节点，开始提供rpc远程网络调用服务
	void Run();

private:
	// 新的socket连接回调
	void OnConnection(muduo::net::TcpConnectionPtr const &);
	// 已建立连接用户的读写事件回调
	void OnMessage(muduo::net::TcpConnectionPtr const &, muduo::net::Buffer *, muduo::Timestamp);
	// Closure的回调操作，用于序列化rpc的响应和网络发送
	void SendRpcResponse(muduo::net::TcpConnectionPtr const &, google::protobuf::Message *);

	// service服务类型信息
	struct ServiceInfo {
		google::protobuf::Service *m_service;													 // 保存服务对象
		std::unordered_map<std::string, google::protobuf::MethodDescriptor const *> m_methodMap; // 保存服务方法
	};

	// 存储注册成功的服务对象和其服务方法的所有信息
	std::unordered_map<std::string, ServiceInfo> m_serviceMap;

	// EventLoop事件循环
	muduo::net::EventLoop m_eventLoop;
};

#endif // MYPRC_RPCPROVIDER_H
