/*
 * author : https://github.com/UnsterblichW
 * brief  : rpc zookeeper模块的客户端相关操作的封装
 */

#ifndef MYPRC_ZKCLIENTWAPPER_H
#define MYPRC_ZKCLIENTWAPPER_H

#include <semaphore.h>
#include <string>
#include <zookeeper/zookeeper.h>

// 封装的zk客户端类
class ZKClient {
public:
	ZKClient();
	~ZKClient();
	// zkclient启动连接zkserver
	void Start();
	// 在zkserver上根据指定的path创建znode节点
	void Create(char const *path, char const *data, int datalen, int state = 0);
	// 根据参数指定的znode节点路径，或者znode节点的值
	std::string GetData(char const *path);

private:
	// zk的客户端句柄
	zhandle_t *m_zhandle;
};
#endif // MYPRC_ZKCLIENTWAPPER_H
