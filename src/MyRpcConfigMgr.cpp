/*
 * author : https://github.com/UnsterblichW
 * brief  : rpc 配置管理器
 */

#include "MyRpcConfigMgr.hpp"
#include <fstream>
// #include <iostream>
#include <stdexcept>

// 构造函数，可以选择传入配置文件路径
// MyRpcConfigMgr::MyRpcConfigMgr(std::string const &configFile) {
// 	loadConfigFile(configFile);
// }

// 获取 RPC 服务器的 IP 地址
std::string_view MyRpcConfigMgr::getRpcServerIp() const {
	return getConfigValue("rpcserverip");
}

// 获取 RPC 服务器的端口号
int MyRpcConfigMgr::getRpcServerPort() const {
	return std::stoi(std::string(getConfigValue("rpcserverport")));
}

// 获取 Zookeeper 的 IP 地址
std::string_view MyRpcConfigMgr::getZookeeperIp() const {
	return getConfigValue("zookeeperip");
}

// 获取 Zookeeper 的端口号
int MyRpcConfigMgr::getZookeeperPort() const {
	return std::stoi(std::string(getConfigValue("zookeeperport")));
}

// 从配置文件中读取键值对并存储到 configMap 中
void MyRpcConfigMgr::loadConfigFile(std::string const &configFile) {
	std::ifstream infile(configFile);
	if (!infile.is_open()) {
		throw std::runtime_error("Could not open config file: " + configFile);
	}
	std::string line;
	while (std::getline(infile, line)) {
		// 忽略注释行或空行
		if (line.empty() || line[0] == '#') {
			continue;
		}
		// 查找等号的位置
		auto delimiterPos = line.find('=');
		if (delimiterPos != std::string::npos) {
			std::string key = line.substr(0, delimiterPos);
			std::string value = line.substr(delimiterPos + 1);
			// 删除key和value中的空格
			key = trim(key);
			value = trim(value);
			// 存储到配置映射表中
			configMap[key] = value;
		}
	}
	infile.close();
}

// 从配置映射表中获取值
std::string_view MyRpcConfigMgr::getConfigValue(std::string_view key) const {
	auto it = configMap.find(std::string(key));
	if (it != configMap.end()) {
		return it->second;
	} else {
		throw std::runtime_error("Config key not found: " + std::string(key));
	}
}

// 辅助函数：去除字符串视图两端的空格
std::string_view MyRpcConfigMgr::trim(std::string_view str) const {
	char const *whitespace = " \t\n\r\f\v";
	size_t const begin = str.find_first_not_of(whitespace);
	if (begin == std::string_view::npos) {
		return {}; // 只包含空白字符的字符串
	}
	size_t const end = str.find_last_not_of(whitespace);
	return str.substr(begin, end - begin + 1);
}

// int main() {
// 	try {
// 		MyRpcConfigMgr config("rpcconfig.conf");

// 		std::cout << "RPC Server IP: " << config.getRpcServerIp() << std::endl;
// 		std::cout << "RPC Server Port: " << config.getRpcServerPort() << std::endl;
// 		std::cout << "Zookeeper IP: " << config.getZookeeperIp() << std::endl;
// 		std::cout << "Zookeeper Port: " << config.getZookeeperPort() << std::endl;
// 	} catch (std::exception const &e) {
// 		std::cerr << "Error: " << e.what() << std::endl;
// 		return EXIT_FAILURE;
// 	}

// 	return EXIT_SUCCESS;
// }
