#include "FRPRunnable.h"

#include <string>

#include "BaseLib.h"
#include "FRPSubsystem.h"
#include "NetLib.h"
#include "SocketSubsystem.h"
#include "Kismet/KismetStringLibrary.h"
#include "UserDefine.h"


Client::Client(const FString& id)
{
    //设置ID并创建客户端Socket
    //Set ID and create client socket
    ID = id;
    Socket = UNetLib::MakeSocket(0, false);
    UE_LOG(LogTemp, Warning, TEXT("FRPLOG >> New Client [%s] Connect"), *ID);
}

Client::~Client()
{
    //关闭删除Socket
    //Close and delete socket
    UNetLib::CloseSocket(Socket);
    UE_LOG(LogTemp, Warning, TEXT("FRPLOG >> Client [%s] Disconnect"), *ID);
}

bool Client::IsMe(const FString& in_id)
{
    return in_id == ID;
}

void Client::ResumeTime()
{
    Time = CLIENT_DESTROY_TIME;
}

void Client::SubTime()
{
    Time--;
}

bool Client::IsDead()
{
    return Time <= 0;
}

FFRPRunnable::FFRPRunnable(UFRPSubsystem* subsys)
{
    //设置Subsystem引用并创建线程
    //Set subsystem reference and create thread
    Subsystem = subsys;
    Thread = FRunnableThread::Create(this,TEXT("FRPRunnable"));
}

FFRPRunnable::~FFRPRunnable()
{
    //关闭线程
    //Close thread
    if (Thread)
    {
        Thread->Kill();
        delete Thread;
        Thread = nullptr;
    }
}

bool FFRPRunnable::Init()
{
    //调用BeginPlay
    //Call BeginPlay
    UE_LOG(LogTemp, Warning, TEXT("FRPLOG >> FRP Thread Init"));
    BeginPlay();
    return true;
}

uint32 FFRPRunnable::Run()
{
    //计算DeltaTime并调用Tick
    //Calculate DeltaTime and call Tick
    UE_LOG(LogTemp, Warning, TEXT("FRPLOG >> FRP Thread Loop"));
    LastTime = FPlatformTime::Seconds();
    while (IsRunning)
    {
        const double cur_time = FPlatformTime::Seconds();
        const double delta_time = cur_time - LastTime;
        LastTime = cur_time;
        Tick(delta_time);
    }
    return 0;
}

void FFRPRunnable::Stop()
{
    IsRunning = false;
}

void FFRPRunnable::Exit()
{
    UE_LOG(LogTemp, Warning, TEXT("FRPLOG >> FRP Thread Exit"));
    EndPlay();
}

void FFRPRunnable::BeginPlay()
{
    //创建App通信Socket
    //Create App communication socket
    AppSocket = UNetLib::MakeSocket(0, false);
}

void FFRPRunnable::Tick(const double& delta_time)
{
    //调用Tick下的事件
    //Call events under Tick
    TickRecvApp();
    TickRecvClient();
    TickTimer(delta_time);
}

void FFRPRunnable::EndPlay()
{
    //关闭App通信Socket
    //Close App communication socket
    UNetLib::CloseSocket(AppSocket);
    //删除所有客户端
    //Delete all clients
    for (Client* cli : ClientList)
    {
        delete cli;
    }
}

void FFRPRunnable::TickRecvApp()
{
    //接收App的消息
    //Receive messages from App
    uint8 data[2048];
    int bytes = 0;
    TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    if (UNetLib::Recv(AppSocket, data, sizeof(data), addr, bytes))
    {
        //交给HandleRecvApp接管
        //Hand over to HandleRecvApp
        HandleRecvApp(data, bytes, addr.Get());
    }
}

void FFRPRunnable::TickRecvClient()
{
    //遍历接收所有客户端的消息
    //Iterate and receive messages from all clients
    uint8 data[2048];
    int bytes = 0;
    TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    for (Client* cli : ClientList)
    {
        if (UNetLib::Recv(cli->Socket, data, sizeof(data), addr, bytes))
        {
            //把每一个都交给HandleRecvClient接管
            //Hand each one over to HandleRecvClient
            HandleRecvClient(cli, data, bytes, addr.Get());
        }
    }
}

void FFRPRunnable::TickTimer(const double& delta_time)
{
    //累加Timer，每秒执行一次
    //Accumulate timer, execute once per second
    TimerValue += delta_time;
    if (TimerValue < 1.0f)
    {
        return;
    }
    TimerValue = 0.0f;

    //遍历客户端列表减少计时
    //Iterate client list and decrease timer
    for (int32 i = ClientList.Num() - 1; i >= 0; --i)
    {
        Client* cli = ClientList[i];
        cli->SubTime();
        //如果客户端死亡则移出列表并删除
        //If client is dead, remove from list and delete
        if (cli->IsDead())
        {
            delete cli;
            ClientList.RemoveAtSwap(i);
        }
    }

    //如果是服务器
    //If server
    if (Subsystem->IsServer)
    {
        //如果玩家ID为空则向App请求ID"@id"
        //If PlayerID is empty, request ID from App "@id"
        if (PlayerID == "")
        {
            UNetLib::Send(AppSocket, UNetLib::MakeAddr(Subsystem->BaseIP, Subsystem->AppPort).Get(),TEXT("@id"));
            return;
        }

        //如果Session启动了则向App发送注册信息"@reg/密码/属性+附加属性\0"
        //If session is open, send registration info "@reg/password/property+append\0"
        if (Subsystem->SessionOpen)
        {
            const FString reg_send = "@reg/" + PlayerID + "/" + Subsystem->Password + "/" + Subsystem->Property + Subsystem->PropertyAppend + "\0";
            UNetLib::Send(AppSocket, UNetLib::MakeAddr(Subsystem->BaseIP, Subsystem->AppPort).Get(), reg_send);

            //如果OwnerPort有效则发送"@imowner"
            //If OwnerPort is valid, send "@imowner"
            if (OwnerPort != -1)
            {
                const FString own_send = TEXT("@imowner");
                UNetLib::Send(AppSocket, UNetLib::MakeAddr(Subsystem->BaseIP, OwnerPort).Get(), own_send);
            }
        }
        else
        {
            OwnerPort = -1;
        }
    }
}

void FFRPRunnable::HandleRecvApp(const uint8* data, const int& bytes, const FInternetAddr& addr)
{
    //创建一个临时FString并分割为参数
    //Create a temporary FString and split into parameters
    FString temp_str = FString(bytes,UTF8_TO_TCHAR((const char*)data));
    TArray<FString> param = UBaseLib::SplitString(temp_str, '/');

    //参数<=0则抛弃
    //Discard if parameters <= 0
    if (param.Num() <= 0)
    {
        return;
    }

    //如果参数[0]为"@id"则交给DoID接管
    //If param[0] is "@id", hand over to DoID
    if (param[0] == TEXT("@id") && param.Num() == 2)
    {
        DoID(data, bytes, addr, param);
        return;
    }

    //如果参数[0]为"@os"则交给DoOS接管
    //If param[0] is "@os", hand over to DoOS
    if (param[0] == TEXT("@os") && param.Num() == 2)
    {
        DoOS(data, bytes, addr, param);
        return;
    }

    //如果不是以上情况则对收到的字节进行分割
    //Otherwise split received bytes
    const uint8* left;
    int left_len;
    const uint8* right;
    int right_len;
    if (UBaseLib::SplitByte(data, bytes, '/', left, left_len, right, right_len))
    {
        //左半部分是客户端对应ID
        //Left part is client ID
        FString temp_id = FString(left_len,UTF8_TO_TCHAR(reinterpret_cast<const char*>(left)));
        Client* cur_client = nullptr;

        //遍历查找客户端
        //Iterate to find client
        for (Client* cli : ClientList)
        {
            if (cli->IsMe(temp_id))
            {
                cur_client = cli;
                UNetLib::Send(cli->Socket, UNetLib::MakeAddr("127.0.0.1", Subsystem->SocketPort).Get(), right, right_len);
                break;
            }
        }

        //如果客户端不存在则创建
        //If client does not exist, create one
        if (!cur_client)
        {
            cur_client = new Client(temp_id);
            cur_client->ResumeTime();
            ClientList.Emplace(cur_client);
        }

        //重置客户端计时
        //Reset client timer
        cur_client->ResumeTime();
    }
}

void FFRPRunnable::DoID(const uint8* data, const int& bytes, const FInternetAddr& addr, const TArray<FString>& param)
{
    //判断玩家ID是否为空，如果是则设置ID
    //Check if PlayerID is empty, if so set ID
    if (PlayerID != "")
    {
        return;
    }
    PlayerID = param[1];
    UE_LOG(LogTemp, Warning, TEXT("FRPLOG >> Set PlayerID [%s]"), *PlayerID);
}

void FFRPRunnable::DoOS(const uint8* data, const int& bytes, const FInternetAddr& addr, const TArray<FString>& param)
{
    //设置主机端发送Port
    //Set host-side sending port
    OwnerPort = UKismetStringLibrary::Conv_StringToInt(param[1]);
}

void FFRPRunnable::HandleRecvClient(Client* client, const uint8* data, const int& bytes, const FInternetAddr& addr)
{
    //将客户端消息加上他的ID进行发送
    //Send client message with its ID prefixed
    FString add_str = client->ID + "/";
    std::string std_str(TCHAR_TO_UTF8(*add_str));
    const char* cstr = std_str.c_str();
    UNetLib::Send(AppSocket, UNetLib::MakeAddr(Subsystem->BaseIP, OwnerPort).Get(), cstr, data, bytes);
}
