/*
 * author : https://github.com/UnsterblichW
 * brief  : 一些通用的结构定义
 */

#ifndef MYRPC_COMMON_H
#define MYRPC_COMMON_H

class noncopyable {
public:
	noncopyable(noncopyable const &) = delete;
	void operator=(noncopyable const &) = delete;

protected:
	noncopyable() = default;
	~noncopyable() = default;
};

#endif // MYRPC_COMMON_H
