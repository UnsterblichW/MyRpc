/*
 * author : https://github.com/UnsterblichW
 * brief  : rpc 服务控制器
 */

#ifndef MYPRC_RPCCONTROLLER_H
#define MYPRC_RPCCONTROLLER_H

#include <google/protobuf/service.h>
#include <string>

class MyRpcController : public google::protobuf::RpcController {
public:
	MyRpcController();
	void Reset() override;
	bool Failed() const override;
	std::string ErrorText() const override;
	void SetFailed(std::string const &reason) override;

	// 目前未实现具体的功能
	void StartCancel() override;
	bool IsCanceled() const override;
	void NotifyOnCancel(google::protobuf::Closure *callback) override;

private:
	bool m_failed;		   // RPC方法执行过程中的状态
	std::string m_errText; // RPC方法执行过程中的错误信息
};

#endif // MYPRC_RPCCONTROLLER_H
