/*
 * author : https://github.com/UnsterblichW
 * brief  : rpc 配置管理器
 */
#ifndef MYPRC_CONFIGMGR_H
#define MYPRC_CONFIGMGR_H

#include <string>
#include <string_view>
#include <unordered_map>

class MyRpcConfigMgr {
public:
	// 构造函数，可以选择传入配置文件路径
	// MyRpcConfigMgr(std::string const &configFile);

	// 从配置文件中读取键值对并存储到 configMap 中
	void loadConfigFile(std::string const &configFile);

	// 获取 RPC 服务器的 IP 地址
	std::string_view getRpcServerIp() const;

	// 获取 RPC 服务器的端口号
	int getRpcServerPort() const;

	// 获取 Zookeeper 的 IP 地址
	std::string_view getZookeeperIp() const;

	// 获取 Zookeeper 的端口号
	int getZookeeperPort() const;

private:
	std::unordered_map<std::string, std::string> configMap;

	// 从配置映射表中获取值
	std::string_view getConfigValue(std::string_view key) const;

	// 辅助函数：去除字符串视图两端的空格
	std::string_view trim(std::string_view str) const;
};

#endif // MYPRC_CONFIGMGR_H
