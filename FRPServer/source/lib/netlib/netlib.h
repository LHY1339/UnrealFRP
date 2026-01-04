#pragma once

#include "lib/header/net.h"
#include "lib/header/std.h"

#pragma comment(lib,"ws2_32.lib")

class netlib
{
public:
    //初始化WSADATA
    //initialize wsadata
    static void init_wsadata(WSADATA& wsa_data);

    //创建udpsocket
    //create udp socket
    static SOCKET make_socket(const int& port, const bool& block = true);

    //创建addr
    //create addr
    static sockaddr_in make_addr(const char* ip, const int& port);

    //获取addr的ip
    //get ip from addr
    static std::string get_ip(const sockaddr_in& addr);

    //获取socket的ip
    //get ip from socket
    static std::string get_ip(const SOCKET& sock);

    //获取addr的port
    //get port from addr
    static int get_port(const sockaddr_in& addr);

    //获取socket的port
    //get port from socket
    static int get_port(const SOCKET& sock);

    //获取socket的addr
    //get addr from socket
    static sockaddr_in get_addr(const SOCKET& sock);

    //判断两个addr是否相等
    //check if two addr are equal
    static bool equal(const sockaddr_in& addr1, const sockaddr_in& addr2);

    //接收udp消息
    //receive udp message
    static bool recv(const SOCKET& sock, char* buffer, int buffer_len, sockaddr_in& addr, int& recvlen);

    //发送udp消息
    //send udp message
    static void send(const SOCKET& sock, const sockaddr_in& addr, const char* data, int data_len);

    //发送udp附加消息
    //send udp message with additional data
    static void send(const SOCKET& sock, const sockaddr_in& addr, const char* add, const char* data, int data_len);
};
