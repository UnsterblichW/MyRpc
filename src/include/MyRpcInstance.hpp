/*
 * author : https://github.com/UnsterblichW
 * brief  : rpc 服务的实例类
 */

#ifndef MYPRC_RPCINSTANCE_H
#define MYPRC_RPCINSTANCE_H

// #include "mprpcchannel.h"
// #include "mprpccontroller.h"
#include "MyRpcConfigMgr.hpp"
#include "common.hpp"

// rpc框架的实例，负责框架的一些初始化操作，单例，不允许复制
class MyRpcInstance : noncopyable {
public:
	static void Init(int argc, char **argv);
	static MyRpcInstance &getInstance();
	static MyRpcConfigMgr &getConfig();

private:
	static MyRpcConfigMgr m_config;

	MyRpcInstance() {}

	MyRpcInstance(MyRpcInstance const &) = delete;
	MyRpcInstance(MyRpcInstance &&) = delete;
};

#endif // MYPRC_RPCINSTANCE_H
