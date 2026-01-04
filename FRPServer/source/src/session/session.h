#pragma once
#include "lib/header/net.h"
#include "lib/header/std.h"
#include "src/client/client.h"

class session
{
public:
    //入口函数
    //entry function
    void run();

    //删除函数
    //destroy function
    void destroy();

    //判断是否是这个客户端
    //check if this is the client
    bool is_me(const std::string& _id);

    //重置计时
    //reset timer
    void resume_time();

    //减少计时
    //decrease timer
    void sub_time();

    //判断是否死亡
    //check if dead
    bool is_dead();

    //计时器调用
    //timer call
    void do_timer();

    //序列化为string
    //serialize to string
    std::string to_string();

private:
    //线程主函数
    //thread main function
    void domain();

    //owner_socket接管函数
    //owner_socket handle function
    void owner_handle(const char* buffer, int recvlen, const sockaddr_in& addr);

    //listen_socket接管函数
    //listen_socket handle function
    void listen_handle(const char* buffer, int recvlen, const sockaddr_in& addr);

    //创建新的client_id
    //create new client_id
    int new_client_id();

public:
    //接收主机端消息
    //receive host messages
    SOCKET owner_socket = INVALID_SOCKET;

    //接收客户端消息
    //receive client messages
    SOCKET listen_socket = INVALID_SOCKET;

    //session的id
    //session id
    std::string id = "";

    //密码
    //password
    std::string password = "";

    //属性（可自定义协议）
    //property (customizable protocol)
    std::string property = "";

    //主函数线程
    //main function thread
    std::thread th_domain;

    //主机端的addr
    //host addr
    sockaddr_in owner_addr = sockaddr_in{};

private:
    //计时
    //timer
    int time = 0;

    //运行标识符
    //running flag
    bool is_running = true;

    //客户端id
    //client id
    int client_id = 10;

    //客户端列表
    //client list
    std::vector<client*> client_list;
};
