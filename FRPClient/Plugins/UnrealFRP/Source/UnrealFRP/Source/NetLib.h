#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "NetLib.generated.h"

UCLASS()
class UNREALFRP_API UNetLib : public UObject
{
	GENERATED_BODY()

public:
	//创建Socket
	//Create a socket
	static FSocket* MakeSocket(int port, bool block);
    
	//关闭Socket
	//Close a socket
	static void CloseSocket(FSocket* socket);

	//创建Addr
	//Create an address
	static TSharedRef<FInternetAddr> MakeAddr(const FString& ip, const int& port);

	//获取Addr的IP
	//Get IP from address
	static FString GetIP(const FInternetAddr& addr);

	//获取Socket的IP
	//Get IP from socket
	static FString GetIP(FSocket* sock);

	//获取Addr的Port
	//Get port from address
	static int GetPort(const FInternetAddr& addr);

	//获取Socket的Port
	//Get port from socket
	static int GetPort(FSocket* sock);

	//获取Socket的Addr
	//Get address from socket
	static TSharedRef<FInternetAddr> GetAddr(FSocket* sock);

	//判断两个Addr是否相等
	//Check whether two addresses are equal
	static bool Equal(const FInternetAddr& a, const FInternetAddr& b);

	//接收消息
	//Receive message
	static bool Recv(FSocket* sock, uint8* buffer, int buffer_len, TSharedRef<FInternetAddr>& addr, int& recvlen);

	//发送data数据
	//Send raw data
	static void Send(FSocket* sock, const FInternetAddr& addr, const uint8* data, int data_len);

	//发送添加data数据（add+data）
	//Send prefixed data (add + data)
	static void Send(FSocket* sock, const FInternetAddr& addr, const char* add, const uint8* data, int data_len);

	//发送FString
	//Send FString
	static void Send(FSocket* sock, const FInternetAddr& addr, const FString& str);

	//判断是否是多人主机端
	//Check whether this is a multiplayer server
	static bool IsServer(const UObject* WorldContextObject);

	//获取IPNetDriver的ListenSocket
	//Get ListenSocket from IPNetDriver
	static FSocket* GetDriverSocket(const UWorld* World);
};
