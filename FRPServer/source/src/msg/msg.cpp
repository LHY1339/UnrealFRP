#include "msg.h"
#include "src/global/gdefine.h"
#include "lib/baselib/baselib.h"
#include "lib/netlib/netlib.h"
#include "src/app/app.h"

//IO上下文
//io context
struct PER_IO_CONTEXT
{
    OVERLAPPED overlapped{};
    WSABUF wsa_buf;
    char buffer[MSG_BUFFER];
};

//socket上下文
//socket context
struct PER_SOCKET_CONTEXT
{
    SOCKET socket;
    PER_IO_CONTEXT io_context;
};

msg::msg(app* _app)
{
    //设置app实例
    //set app instance
    app_inst = _app;
}

void msg::run()
{
    //初始化并监听socket
    //initialize and listen socket
    init_socket();
    listen(listen_socket, SOMAXCONN);

    //绑定到iocp
    //bind to iocp
    HANDLE h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);

    //创建iocp工作线程
    //create iocp worker threads
    for (int i = 0; i < MSG_THREAD; ++i)
    {
        CreateThread(nullptr, 0, worker_thread, h_iocp, 0, nullptr);
    }

    //循环accept客户端
    //loop accept clients
    while (true)
    {
        SOCKET client_socket = accept(listen_socket, nullptr, nullptr);
        //printf("msg client [%d] connect\n", (int)client_socket);

        auto* socket_context = new PER_SOCKET_CONTEXT();
        socket_context->socket = client_socket;

        CreateIoCompletionPort((HANDLE)client_socket, h_iocp, (ULONG_PTR)socket_context, 0);

        auto& io_con = socket_context->io_context;
        ZeroMemory(&io_con.overlapped, sizeof(OVERLAPPED));
        io_con.wsa_buf.buf = io_con.buffer;
        io_con.wsa_buf.len = MSG_BUFFER;

        DWORD flags = 0;
        DWORD bytes = 0;
        WSARecv(client_socket, &io_con.wsa_buf, 1, &bytes, &flags, &io_con.overlapped, nullptr);
    }
}

void msg::init_socket()
{
    //创建socket
    //create socket
    listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket == INVALID_SOCKET)
    {
        baselib::error(TEXT("listen_socket == INVALID_SOCKET"), TEXT("msg::init_socket"));
    }

    //创建addr
    //create addr
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(MSG_PORT);

    //绑定到addr
    //bind to addr
    if (bind(listen_socket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        baselib::error(TEXT("bind(listen_socket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR"), TEXT("msg::init_socket"));
    }

    printf("<msg> run at port [%d]\n", netlib::get_port(listen_socket));
}

DWORD __stdcall msg::worker_thread(LPVOID iocp)
{
    //设置iocp句柄
    //set iocp handle
    HANDLE h_iocp = (HANDLE)iocp;

    while (true)
    {
        //传输字节数
        //bytes transferred
        DWORD bytes_transferred = 0;
        //完成键
        //completion key
        ULONG_PTR completion_key = 0;
        //OVERLAPPED指针
        //overlapped pointer
        LPOVERLAPPED overlapped = nullptr;

        //从完成端口队列中取出一个事件
        //get event from completion port queue
        BOOL ok = GetQueuedCompletionStatus(
            h_iocp,
            &bytes_transferred,
            &completion_key,
            &overlapped,
            INFINITE
        );

        //还原出socket上下文和IO上下文
        //restore socket context and io context
        auto* socket_context = (PER_SOCKET_CONTEXT*)completion_key;
        auto* io_context = CONTAINING_RECORD(overlapped, PER_IO_CONTEXT, overlapped);

        //如果失败或对端关闭连接
        //if failed or peer closed connection
        if (!ok || bytes_transferred == 0)
        {
            //printf("msg client [%d] disconnect\n", (int)socket_context->socket);
            closesocket(socket_context->socket);
            delete socket_context;
            continue;
        }

        //转到接管函数
        //forward to handle function
        handle_message(socket_context->socket, io_context->buffer, bytes_transferred);

        //重置OVERLAPPED结构
        //reset overlapped structure
        ZeroMemory(&io_context->overlapped, sizeof(OVERLAPPED));
        DWORD flags = 0;
        DWORD bytes = 0;

        //投递异步接收请求
        //post async receive request
        WSARecv(
            socket_context->socket,
            &io_context->wsa_buf,
            1,
            &bytes,
            &flags,
            &io_context->overlapped,
            nullptr
        );
    }
    return 0;
}

void msg::handle_message(SOCKET sock, char* data, DWORD bytes)
{
    //分割接收到的数据
    //split received data
    std::string msg(data, bytes);
    std::vector<std::string> param = baselib::split_string(msg, '/');

    //如果参数数量<=0则直接抛弃
    //discard if param size <= 0
    if (param.size() <= 0)
    {
        return;
    }

    //如果为@get_list则获取session列表然后发回
    //if @get_list then get session list and send back
    if (param[0] == "@get_list")
    {
        std::string sendback = "@list/" + app_inst->to_string() + "\0";
        send_with_head(sock, sendback);
        return;
    }
    //如果为@ask_addr则尝试获取对应id的addr
    //if @ask_addr then try to get addr by id
    if (param[0] == "@ask_addr" && param.size() == 3)
    {
        std::string sendback = "@addr/" + app_inst->ask_addr(param[1], param[2]) + "\0";
        send_with_head(sock, sendback);
        return;
    }

    printf("msg error recv : %s\n", msg.c_str());
}

void msg::send_with_head(SOCKET sock, const std::string& msg)
{
    //获取消息长度
    //get message length
    uint32_t len = (uint32_t)msg.size();

    //写入4字节消息长度
    //write 4-byte message length
    uint8_t header[4];
    memcpy(header, &len, 4);

    //发送长度
    //send length
    send(sock, (const char*)header, 4, 0);
    //发送消息
    //send message
    send(sock, msg.data(), len, 0);
}
