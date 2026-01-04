#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "HAL/RunnableThread.h"
#include "Async/Async.h"
#include "IPAddress.h"

class UFRPSubsystem;

class Client
{
public:
    //构造Client
    //Construct Client
    Client(const FString& id);

    //析构Client
    //Destruct Client
    ~Client();

    //判断是否是相同Client
    //Check whether it is the same Client
    bool IsMe(const FString& in_id);

    //重置死亡计时
    //Reset death timer
    void ResumeTime();

    //减去死亡计时
    //Decrease death timer
    void SubTime();

    //判断是否死亡
    //Check whether dead
    bool IsDead();

public:
    //客户端主Socket
    //Client main socket
    FSocket* Socket;

    //客户端ID
    //Client ID
    FString ID;

    //计时器
    //Timer
    int Time = 0;
};

class FFRPRunnable : public FRunnable
{
public:
    //构造FRPRunnable并创建线程
    //Construct FFRPRunnable and create thread
    FFRPRunnable(UFRPSubsystem* subsys);

    //析构FRPRunnable并关闭线程
    //Destruct FFRPRunnable and close thread
    ~FFRPRunnable();

    //继承Runnable函数
    //Override Runnable functions
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;
    virtual void Exit() override;

private:
    //启动时线程内调用
    //Called inside thread at start
    void BeginPlay();
    
    //每帧调用
    //Called every frame
    void Tick(const double& delta_time);

    //线程退出时线程内调用
    //Called inside thread on exit
    void EndPlay();

    //接收App的消息
    //Receive messages from App
    void TickRecvApp();

    //接收客户端发回的消息
    //Receive messages sent back from client
    void TickRecvClient();

    //减少客户端计时器
    //Decrease client timers
    void TickTimer(const double& delta_time);

    //接管App消息
    //Handle App messages
    void HandleRecvApp(const uint8* data, const int& bytes, const FInternetAddr& addr);

    //接管@id事件
    //Handle @id event
    void DoID(const uint8* data, const int& bytes, const FInternetAddr& addr, const TArray<FString>& param);

    //接管@os事件
    //Handle @os event
    void DoOS(const uint8* data, const int& bytes, const FInternetAddr& addr, const TArray<FString>& param);

    //接管客户端消息
    //Handle client messages
    void HandleRecvClient(Client* client, const uint8* data, const int& bytes, const FInternetAddr& addr);
    
private:
    //线程
    //Thread
    FRunnableThread* Thread;

    //运行标志位
    //Running flag
    FThreadSafeBool IsRunning = true;

    //上次Tick的时间
    //Last tick time
    double LastTime;

    //玩家ID
    //Player ID
    FString PlayerID = "";

    //主机端消息Port
    //Host message port
    int32 OwnerPort = -1;

    //App通信Socket
    //App communication socket
    FSocket* AppSocket;

    //客户端列表
    //Client list
    TArray<Client*> ClientList;

    //Subsystem引用
    //Subsystem reference
    UFRPSubsystem* Subsystem;

    //定时器累计时间
    //Accumulated timer value
    double TimerValue = 0.0f;
};