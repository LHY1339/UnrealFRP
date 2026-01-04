#include "FRPLibrary.h"

#include "FRPSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Kismet/KismetSystemLibrary.h"

UFRPSubsystem* UFRPLibrary::GetSubsystem(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}
	//获取当前UWorld，判断UWorld和GameInstance是否存在
	//Get the current UWorld and check whether UWorld and GameInstance exist
	UWorld* world = WorldContextObject->GetWorld();
	if (!world || !world->GetGameInstance())
	{
		return nullptr;
	}
	//获取GameInstance下的Subsystem
	//Get the Subsystem under the GameInstance
	return world->GetGameInstance()->GetSubsystem<UFRPSubsystem>();
}

void UFRPLibrary::FlushFRPSessionList(const UObject* WorldContextObject)
{
	UFRPSubsystem* subsys = GetSubsystem(WorldContextObject);
	subsys->FlushSessionList();
}

void UFRPLibrary::BindOnSessionListUpdate(UObject* WorldContextObject, FOnSessionUpdate Delegate)
{
	UFRPSubsystem* subsys = GetSubsystem(WorldContextObject);
	subsys->OnSessionUpdate = Delegate;
}

void UFRPLibrary::UnbindOnSessionListUpdate(UObject* WorldContextObject)
{
	UFRPSubsystem* subsys = GetSubsystem(WorldContextObject);
	subsys->OnSessionUpdate.Unbind();
}

TArray<FFRPSession> UFRPLibrary::GetSessionList(UObject* WorldContextObject)
{
	UFRPSubsystem* subsys = GetSubsystem(WorldContextObject);
	return subsys->SessionList;
}

void UFRPLibrary::SetAppAddress(UObject* WorldContextObject, int Port)
{
	UFRPSubsystem* subsys = GetSubsystem(WorldContextObject);
	subsys->AppPort = Port;
}

void UFRPLibrary::SetMsgAddress(UObject* WorldContextObject, int Port)
{
	UFRPSubsystem* subsys = GetSubsystem(WorldContextObject);
	subsys->MsgPort = Port;
}

void UFRPLibrary::SetBaseIP(UObject* WorldContextObject, FString IP)
{
	UFRPSubsystem* subsys = GetSubsystem(WorldContextObject);
	subsys->BaseIP = IP;
}

void UFRPLibrary::OpenSession(UObject* WorldContextObject)
{
	UFRPSubsystem* subsys = GetSubsystem(WorldContextObject);
	subsys->SessionOpen = true;
}

void UFRPLibrary::CloseSession(UObject* WorldContextObject)
{
	UFRPSubsystem* subsys = GetSubsystem(WorldContextObject);
	subsys->SessionOpen = false;
}

void UFRPLibrary::SetSessionProperty(UObject* WorldContextObject, FString Property)
{
	UFRPSubsystem* subsys = GetSubsystem(WorldContextObject);
	subsys->Property = Property;
}

void UFRPLibrary::SetSessionPassword(UObject* WorldContextObject, FString Password)
{
	UFRPSubsystem* subsys = GetSubsystem(WorldContextObject);
	subsys->Password = Password;
}

void UFRPLibrary::SetSessionPropertyAppend(UObject* WorldContextObject, FString PropertyAppend)
{
	UFRPSubsystem* subsys = GetSubsystem(WorldContextObject);
	subsys->PropertyAppend = PropertyAppend;
}

void UFRPLibrary::AskForAddress(UObject* WorldContextObject, const FString& ID, const FString& Password)
{
	UFRPSubsystem* subsys = GetSubsystem(WorldContextObject);
	subsys->AskForAddr(ID, Password);
}

void UFRPLibrary::BindOnAddrUpdate(UObject* WorldContextObject, FOnAddrUpdate Delegate)
{
	UFRPSubsystem* subsys = GetSubsystem(WorldContextObject);
	subsys->OnAddrUpdate = Delegate;
}

void UFRPLibrary::UnbindOnAddrUpdate(UObject* WorldContextObject)
{
	UFRPSubsystem* subsys = GetSubsystem(WorldContextObject);
	subsys->OnAddrUpdate.Unbind();
}

void UFRPLibrary::ConnectSession(UObject* WorldContextObject, FString Addr)
{
	UFRPSubsystem* subsys = GetSubsystem(WorldContextObject);
	//使用"open ip:port"命令进行连接
	//Use the "open ip:port" command to connect
	FString cmd = "open " + subsys->BaseIP + ":" + Addr;
	UKismetSystemLibrary::ExecuteConsoleCommand(WorldContextObject->GetWorld(), cmd);
}
