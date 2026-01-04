#include "client.h"
#include "lib/netlib/netlib.h"
#include "src/global/gdefine.h"

client::client(const int& _id, const std::string& _ses_id)
{
    //设置id和session的id
    //set id and session id
    id = _id;
    session_id = _ses_id;
    printf("session [%s] : client [%d] connect\n", session_id.c_str(), id);
}

client::~client()
{
    printf("session [%s] : client [%d] disconnect\n", session_id.c_str(), id);
}

bool client::is_me(const sockaddr_in& _addr)
{
    return netlib::equal(_addr, addr);
}

void client::resume_time()
{
    //重置死亡计时
    //reset death timer
    left_time = CLIENT_DESTROY_TIME;
}

void client::sub_time()
{
    //减少计时
    //decrease timer
    left_time--;
}

bool client::is_dead()
{
    //判断是否死亡
    //check if dead
    return left_time <= 0;
}
