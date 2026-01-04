#pragma once
#include "lib/header/net.h"
#include "lib/header/std.h"

class app;

class msg
{
public:
    //初始化
    //initialize
    msg(app* _app);

    //入口函数
    //entry function
    void run();

private:
    //初始化tcpsocket
    //initialize tcp socket
    void init_socket();

    //iocp工作线程
    //iocp worker thread
    static DWORD WINAPI worker_thread(LPVOID iocp);

    //接管消息
    //handle message
    static void handle_message(SOCKET sock, char* data, DWORD bytes);

    //发送带4字节头的消息，用于防止粘包拆包
    //send message with 4-byte header to prevent sticky/fragmented packets
    static void send_with_head(SOCKET sock, const std::string& msg);

private:
    //监听socket
    //listening socket
    SOCKET listen_socket = INVALID_SOCKET;

    //app的实例
    //app instance
    inline static app* app_inst = nullptr;
};
