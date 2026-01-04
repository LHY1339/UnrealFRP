#include "src/app/app.h"
#include "src/msg/msg.h"
#include "lib/header/std.h"
#include "lib/netlib/netlib.h"

//实例
//Instance
app* app_inst;
msg* msg_inst;

//app运行线程
//app running thread
void thread_app()
{
	app_inst->run();
}

//msg运行线程
//msg running thread
void thread_msg()
{
	msg_inst->run();
}

//初始化WSADATA
//initialize WSADATA
void init_network()
{
	WSADATA wsa_data;
	netlib::init_wsadata(wsa_data);
}

int main()
{
	//初始化网络
	//initialize network
	init_network();

	//创建实例
	//create instances
	app_inst = new app();
	msg_inst = new msg(app_inst);

	//创建线程
	//create thread
	std::thread(thread_app).detach();
	//这个直接用主线程跑
	//run this function directly on the main thread
	thread_msg();
	return 0;
}