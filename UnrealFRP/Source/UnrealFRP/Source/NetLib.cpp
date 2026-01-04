#include "NetLib.h"

#include "BaseLib.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "IpNetDriver.h"

FSocket* UNetLib::MakeSocket(int port, bool block)
{
	ISocketSubsystem* ss = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	FSocket* sock = ss->CreateSocket(NAME_DGram, TEXT("UNetLib_Socket"), false);
	if (!sock)
	{
		UE_LOG(LogTemp, Error, TEXT("CreateSocket failed"));
		return nullptr;
	}

	sock->SetNonBlocking(!block);
	sock->SetReuseAddr(true);

	TSharedRef<FInternetAddr> addr = ss->CreateInternetAddr();
	addr->SetAnyAddress();
	addr->SetPort(port);

	if (!sock->Bind(*addr))
	{
		UE_LOG(LogTemp, Error, TEXT("Bind failed"));
		ss->DestroySocket(sock);
		return nullptr;
	}

	return sock;
}

void UNetLib::CloseSocket(FSocket* socket)
{
	if (socket)
	{
		socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(socket);
		socket = nullptr;
	}
}

TSharedRef<FInternetAddr> UNetLib::MakeAddr(const FString& ip, const int& port)
{
	ISocketSubsystem* ss = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	TSharedRef<FInternetAddr> addr = ss->CreateInternetAddr();
	bool ok = false;
	addr->SetIp(*ip, ok);
	addr->SetPort(port);

	return addr;
}

FString UNetLib::GetIP(const FInternetAddr& addr)
{
	return addr.ToString(false);
}

FString UNetLib::GetIP(FSocket* sock)
{
	ISocketSubsystem* ss = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	TSharedRef<FInternetAddr> addr = ss->CreateInternetAddr();
	sock->GetAddress(*addr);

	return addr->ToString(false);
}

int UNetLib::GetPort(const FInternetAddr& addr)
{
	return addr.GetPort();
}

int UNetLib::GetPort(FSocket* sock)
{
	if (!sock)
	{
		UE_LOG(LogTemp, Error, TEXT("GetPort failed: sock is nullptr"));
		return -1;
	}

	ISocketSubsystem* ss = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (!ss)
	{
		UE_LOG(LogTemp, Error, TEXT("GetPort failed: SocketSubsystem is nullptr"));
		return -1;
	}

	TSharedRef<FInternetAddr> addr = ss->CreateInternetAddr();

	sock->GetAddress(*addr);

	return addr->GetPort();
}


TSharedRef<FInternetAddr> UNetLib::GetAddr(FSocket* sock)
{
	ISocketSubsystem* ss = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	TSharedRef<FInternetAddr> addr = ss->CreateInternetAddr();
	sock->GetAddress(*addr);

	return addr;
}

bool UNetLib::Equal(const FInternetAddr& a, const FInternetAddr& b)
{
	return a.ToString(true) == b.ToString(true);
}

bool UNetLib::Recv(FSocket* sock, uint8* buffer, int buffer_len, TSharedRef<FInternetAddr>& addr, int& recvlen)
{
	recvlen = 0;
	return sock->RecvFrom(buffer, buffer_len, recvlen, *addr);
}

void UNetLib::Send(FSocket* sock, const FInternetAddr& addr, const uint8* data, int data_len)
{
	int32 sent = 0;
	sock->SendTo(data, data_len, sent, addr);
}

void UNetLib::Send(FSocket* sock, const FInternetAddr& addr, const char* add, const uint8* data, int data_len)
{
	uint8 sendbuf[2048];

	int add_len = strlen(add);
	if (add_len + data_len >= sizeof(sendbuf))
	{
		UE_LOG(LogTemp, Error, TEXT("netlib::send add too long!"));
		return;
	}

	memcpy(sendbuf, add, add_len);
	memcpy(sendbuf + add_len, data, data_len);

	int32 sent = 0;
	sock->SendTo(sendbuf, add_len + data_len, sent, addr);
}

void UNetLib::Send(FSocket* sock, const FInternetAddr& addr, const FString& str)
{
	FTCHARToUTF8 Converter(*str);
	const uint8* data = (const uint8*)Converter.Get();
	int32 data_size = Converter.Length();
	int32 bytes = 0;
	sock->SendTo(data, data_size, bytes, addr);
}

bool UNetLib::IsServer(const UObject* WorldContextObject)
{
	return UKismetSystemLibrary::IsServer(WorldContextObject) && !UKismetSystemLibrary::IsStandalone(WorldContextObject);
}

FSocket* UNetLib::GetDriverSocket(const UWorld* World)
{
	if (!World || !World->NetDriver)
	{
		return nullptr;
	}
	return Cast<UIpNetDriver>(World->NetDriver)->GetSocket();
}
