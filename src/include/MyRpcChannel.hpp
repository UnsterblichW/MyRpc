
/*
 * author : https://github.com/UnsterblichW
 * brief  : rpc框架的客户端所需的相关操作
 */

#ifndef MYPRC_MYRPCCHANNEL_H
#define MYPRC_MYRPCCHANNEL_H

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>

class MyRpcChannel : public google::protobuf::RpcChannel {
public:
	// protobuf生成的_Stub类（比如说example中user.proto生成的UserServiceRpc_Stub类），将会接收一个RpcChannel类的对象，然后调用这个RpcChannel对象实现的CallMethod
	// 通过_Stub类调用proto协议里面与远端定义好了的方法，详见google::protobuf::RpcChannel的注释
	void CallMethod(google::protobuf::MethodDescriptor const *method, google::protobuf::RpcController *controller,
					google::protobuf::Message const *request, google::protobuf::Message *response,
					google::protobuf::Closure *done) override;
};

#endif // MYPRC_MYRPCCHANNEL_H
