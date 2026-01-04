#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "FRPSubsystem.generated.h"

class FFRPRunnable;

USTRUCT(BlueprintType)
struct FFRPSession
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="FRP")
    FString ID = "";

    UPROPERTY(BlueprintReadOnly, Category="FRP")
    bool UsePassword = false;

    UPROPERTY(BlueprintReadOnly, Category="FRP")
    FString Property = "";
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSessionUpdate, const TArray<FFRPSession>&, Session);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAddrUpdate, const bool, Success, const FString&, Addr);

UCLASS()
class UNREALFRP_API UFRPSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    //初始化
    //Initialize
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    //生命周期结束
    //Deinitialize
    virtual void Deinitialize() override;

    //Tick事件
    //Tick event
    virtual void Tick(float DeltaTime) override;

    //是否可以Tick
    //Whether Tick is allowed
    virtual bool IsTickable() const override;

    //获取StatID
    //Get StatID
    virtual TStatId GetStatId() const override;

    //接收消息接管函数
    //Message receive handler
    void OnMsgRecv(FString message, int status);

    //刷新列表
    //Refresh session list
    void FlushSessionList();

    //通过ID和密码向服务器询问端口号
    //Ask server for port using ID and password
    void AskForAddr(const FString& ID, const FString& Pwd);

private:
    //Async发送TCP消息
    //Send TCP message asynchronously
    void SendMsgAsync(const FString& SendMsg);

    //接管"@list"
    //Handle "@list"
    void DoList(const FString& message, const TArray<FString>& param);

    //接管"@addr"
    //Handle "@addr"
    void DoAddr(const FString& message, const TArray<FString>& param);

public:
    //服务器主IP
    //Server main IP
    FString BaseIP = TEXT("");

    //App端口
    //App port
    int32 AppPort = 2233;

    //消息端口
    //Message port
    int32 MsgPort = 2233;

    //是否开启Session
    //Whether session is open
    bool SessionOpen = false;

    //属性
    //Property
    FString Property = TEXT("");

    //密码
    //Password
    FString Password = TEXT("");

    //附加属性（用于做动态属性）
    //Additional property (for dynamic attributes)
    FString PropertyAppend = TEXT("");

    //是否是服务器
    //Whether this is server
    bool IsServer = false;

    //IPNetDriver的ListenSocket的Port
    //ListenSocket port of IPNetDriver
    int SocketPort = -1;

    //Session列表更新委托
    //Session list update delegate
    FOnSessionUpdate OnSessionUpdate;

    //请求Addr更新委托
    //Address request update delegate
    FOnAddrUpdate OnAddrUpdate;

    //Session列表
    //Session list
    TArray<FFRPSession> SessionList;

private:
    //判断是否启用Tick
    //Whether Tick is enabled
    bool TickAble = false;

    //FRPRunnable对象
    //FRPRunnable object
    FFRPRunnable* FRPThread;
};
