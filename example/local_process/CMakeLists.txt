set(SRC_LIST UserServiceCaller.cpp ../user.pb.cc)
# set(SRC_LIST friendservice.cc ../friend.pb.cc)

add_executable(comsumer ${SRC_LIST})
#target_link_libraries(provider mprpc protobuf)
target_link_libraries(comsumer myrpc protobuf)