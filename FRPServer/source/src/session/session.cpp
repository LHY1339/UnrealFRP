#include "session.h"
#include "lib/netlib/netlib.h"
#include "lib/baselib/baselib.h"
#include "src/global/gdefine.h"

void session::run()
{
    //创建非阻塞socket
    //create non-blocking socket
    owner_socket = netlib::make_socket(0, false);
    listen_socket = netlib::make_socket(0, false);

    //打印属性（因为传入为utf8所以要转成gbk打印）
    //print property (convert utf8 to gbk for output)
    std::string property_gb = baselib::utf8_to_gbk(property);
    printf("session [%s] run : [os:%d] [ls:%d] [pass:%s] [prop:%s]\n",
        id.c_str(),
        netlib::get_port(owner_socket),
        netlib::get_port(listen_socket),
        password.c_str(),
        property_gb.c_str()
    );

    //创建线程
    //create thread
    th_domain = std::thread(&session::domain, this);
}

void session::destroy()
{
    //设置标识符为false
    //set flag to false
    is_running = false;
    //关闭socket
    //close socket
    closesocket(owner_socket);
    closesocket(listen_socket);
    //等待线程关闭
    //wait for thread to close
    th_domain.join();
    //删除客户端
    //delete clients
    for (client* cli : client_list)
    {
        delete cli;
    }
    printf("session [%s] destroy\n", id.c_str());
    delete this;
}

bool session::is_me(const std::string& _id)
{
    return id == _id;
}

void session::resume_time()
{
    time = SESSION_DESTROY_TIME;
}

void session::sub_time()
{
    time--;
}

bool session::is_dead()
{
    return time <= 0;
}

void session::do_timer()
{
    //removeif条件
    //removeif condition
    auto remove_cond = [&](client* cli)
        {
            //如果客户端死亡则删除客户端
            //if client is dead then delete
            if (cli->is_dead())
            {
                delete cli;
                return true;
            }
            return false;
        };

    //减少自身计时
    //decrease self timer
    sub_time();
    //遍历客户端减少计时
    //iterate clients and decrease timer
    for (client* cli : client_list)
    {
        cli->sub_time();
    }
    //removeif移除死亡的客户端
    //removeif deletes dead clients
    client_list.erase(std::remove_if(client_list.begin(), client_list.end(), remove_cond), client_list.end());
}

std::string session::to_string()
{
    return id + "&" + (password == "" ? "false" : "true") + "&" + property;
}

void session::domain()
{
    while (is_running)
    {
        //接收消息
        //receive message
        char buffer[2048] = { 0 };
        sockaddr_in addr = sockaddr_in();
        int recvlen = -1;
        if (netlib::recv(owner_socket, buffer, sizeof(buffer), addr, recvlen))
        {
            //如果为主机端则交给owner_handle接管
            //if from host then forward to owner_handle
            owner_handle(buffer, recvlen, addr);
        }
        if (netlib::recv(listen_socket, buffer, sizeof(buffer), addr, recvlen))
        {
            //如果为客户端则交给listen_socket接管
            //if from client then forward to listen_handle
            listen_handle(buffer, recvlen, addr);
        }
    }
}

void session::owner_handle(const char* buffer, int recvlen, const sockaddr_in& addr)
{
    //设置addr为主机端addr
    //set addr as host addr
    owner_addr = addr;
    //如果是自动发送则忽略
    //ignore if auto send
    if (std::string(buffer, recvlen) == "@imowner")
    {
        return;
    }
    //按“/”分割接收消息为两边
    //split received message by "/"
    const char* left;
    int left_len;
    const char* right;
    int right_len;
    if (baselib::split_char(buffer, recvlen, '/', left, left_len, right, right_len))
    {
        //左边为clientid
        //left side is client id
        const std::string cli_id(left, left_len);
        //遍历寻找对应的client
        //iterate to find corresponding client
        for (client* cli : client_list)
        {
            //如果找到则将右边用listen_socket发给客户端
            //if found then send right side to client via listen_socket
            if (cli->id == std::stoi(cli_id))
            {
                netlib::send(listen_socket, cli->addr, right, right_len);
                break;
            }
        }
    }
}

void session::listen_handle(const char* buffer, int recvlen, const sockaddr_in& addr)
{
    //创建一个client空指针
    //create a client null pointer
    client* cur_client = nullptr;
    //遍历寻找client
    //iterate to find client
    for (client* cli : client_list)
    {
        if (cli->is_me(addr))
        {
            cur_client = cli;
            break;
        }
    }
    //如果指针依旧为空（client不在列表里）则创建client
    //if still null (client not in list) then create client
    if (!cur_client)
    {
        cur_client = new client(new_client_id(), id);
        cur_client->addr = addr;
        cur_client->resume_time();
        client_list.emplace_back(cur_client);
    }
    //重置定时器
    //reset timer
    cur_client->resume_time();
    //将clientid加上buffer用owner_socket发送给主机端
    //send client id plus buffer to host via owner_socket
    std::string str = std::to_string(cur_client->id) + "/";
    netlib::send(owner_socket, owner_addr, str.c_str(), buffer, recvlen);
}

int session::new_client_id()
{
    //累加client_id
    //increment client_id
    client_id++;
    return client_id;
}
