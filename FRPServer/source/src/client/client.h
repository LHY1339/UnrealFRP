#pragma once
#include "lib/header/net.h"
#include "lib/header/std.h"

class client
{
public:
    //构造客户端
    //construct client
    client(const int& _id, const std::string& _ses_id);

    //析构客户端
    //destruct client
    ~client();

    //判断是否是这个客户端
    //check if this is the client
    bool is_me(const sockaddr_in& _addr);

    //重置死亡计时
    //reset death timer
    void resume_time();

    //减少计时
    //decrease timer
    void sub_time();

    //判断是否死亡
    //check if dead
    bool is_dead();

public:
    //客户端addr
    //client addr
    sockaddr_in addr = sockaddr_in{};

    //客户端id
    //client id
    int id = -1;

    //session的id
    //session id
    std::string session_id = "";

    //死亡计时
    //death timer
    int left_time = 0;
};
