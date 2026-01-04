#pragma once
#include "lib/header/net.h"
#include "lib/header/std.h"
#include "src/session/session.h"

class app
{
public:
	//入口函数
	//entry function
	void run();

	//序列化到string
	//serialize to string
	std::string to_string();

	//通过id和密码获取端口
	//get port by id and password
	std::string ask_addr(const std::string id, const std::string pass);

private:
	//tick事件
	//tick event
	void tick(const float& delta_time);

	//tick接收数据
	//tick receive data
	void tick_recv();

	//tick定时器
	//tick timer
	void tick_timer(const float& delta_time);

	//接管接收消息函数
	//handle received message function
	void handle(const char* buffer, int recvlen, const sockaddr_in& addr);

	//接管id
	//handle id
	void do_id(const std::vector<std::string>& param, const char* buffer, int recvlen, const sockaddr_in& addr);

	//接管reg
	//handle reg
	void do_reg(const std::vector<std::string>& param, const char* buffer, int recvlen, const sockaddr_in& addr);

	//新建一个player_id
	//create a new player_id
	int new_player_id();

private:
	//app的主socket
	//app main socket
	SOCKET main_socket = INVALID_SOCKET;

	//session列表
	//session list
	std::vector<session*> session_list;

	//玩家id
	//player id
	int player_id = 1000;
};

