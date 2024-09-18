/*
 * author : https://github.com/UnsterblichW
 * brief  : rpc 服务的实例类
 */

#include "MyRpcInstance.hpp"
#include "MyRpcConfigMgr.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

MyRpcConfigMgr MyRpcInstance::m_config;

void ShowArgsHelp() {
	std::cout << "format: command -i <configfile>" << std::endl;
}

void MyRpcInstance::Init(int argc, char **argv) {
	if (argc < 2) {
		ShowArgsHelp();
		exit(EXIT_FAILURE);
	}

	int c = 0;
	std::string config_file;
	while ((c = getopt(argc, argv, "i:")) != -1) {
		switch (c) {
		case 'i': config_file = optarg; break;
		case '?': ShowArgsHelp(); exit(EXIT_FAILURE);
		case ':': ShowArgsHelp(); exit(EXIT_FAILURE);
		default:  break;
		}
	}

	// // 开始加载配置文件了 rpcserver_ip=  rpcserver_port   zookeeper_ip=
	// zookepper_port= m_config.LoadConfigFile(config_file.c_str());

	m_config.loadConfigFile(config_file.c_str());

	std::cout << "rpcserverip:" << m_config.getRpcServerIp() << std::endl;
	std::cout << "rpcserverport:" << m_config.getRpcServerPort() << std::endl;
	std::cout << "zookeeperip:" << m_config.getZookeeperIp() << std::endl;
	std::cout << "zookeeperport:" << m_config.getZookeeperPort() << std::endl;
}

MyRpcInstance &MyRpcInstance::getInstance() {
	static MyRpcInstance instance;
	return instance;
}

MyRpcConfigMgr &MyRpcInstance::getConfig() {
	return m_config;
}
