#include "FRPSubsystem.h"

#include "BaseLib.h"
#include "FRPRunnable.h"
#include "NetLib.h"
#include "Sockets.h"
#include "Interfaces/IPv4/IPv4Address.h"

//TCP返回状态，用于Debug
//TCP return status for debugging
enum ETCPResult
{
	TCP_Success = 0,
	TCP_SocketError = 1,
	TCP_ConnectError = 3,
	TCP_Unknown = 4,
	TCP_RecvError = 5,
	TCP_SendError = 5,
};

void UFRPSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	//创建FRPRunnable对象并且开始Tick
	//Create FRPRunnable object and start Tick
	TickAble = true;
	FRPThread = new FFRPRunnable(this);
}

void UFRPSubsystem::Deinitialize()
{
	Super::Deinitialize();
	//关闭Tick并删除FRPRunnable对象
	//Disable Tick and delete FRPRunnable object
	TickAble = false;
	delete FRPThread;
}

void UFRPSubsystem::Tick(float DeltaTime)
{
	//获取是否是Server和Listen端口
	//Get whether it's server and the listen port
	IsServer = UNetLib::IsServer(GetWorld());
	if (IsServer)
	{
		SocketPort = UNetLib::GetPort(UNetLib::GetDriverSocket(GetWorld()));
	}
	else
	{
		SocketPort = -1;
	}
}

bool UFRPSubsystem::IsTickable() const
{
	return TickAble;
}

TStatId UFRPSubsystem::GetStatId() const
{
	//返回默认StatID
	//Return default StatID
	RETURN_QUICK_DECLARE_CYCLE_STAT(UFRPSubsystem, STATGROUP_Tickables);
}

void UFRPSubsystem::OnMsgRecv(FString message, int status)
{
	UE_LOG(LogTemp, Warning, TEXT("[%d] %s"), status, *message);
	//判断是否接收Success
	//Check whether receive status is Success
	if (status != TCP_Success)
	{
		return;
	}

	//分割消息为参数
	//Split message into parameters
	TArray<FString> param = UBaseLib::SplitString(message, '/');
	//如果参数<=0则抛弃
	//Discard if parameters <= 0
	if (param.Num() <= 0)
	{
		return;
	}

	//如果参数[0]为"@list"则交给DoList处理
	//If param[0] is "@list", handle with DoList
	if (param[0] == "@list")
	{
		DoList(message, param);
		return;
	}

	//如果参数[0]为"@addr"则交给DoList处理
	//If param[0] is "@addr", handle with DoAddr
	if (param[0] == "@addr")
	{
		DoAddr(message, param);
		return;
	}
}

void UFRPSubsystem::FlushSessionList()
{
	SendMsgAsync("@get_list");
}

void UFRPSubsystem::AskForAddr(const FString& ID, const FString& Pwd)
{
	SendMsgAsync("@ask_addr/" + ID + "/" + Pwd);
}

void UFRPSubsystem::SendMsgAsync(const FString& send_msg)
{
	//创建一个WeakPtr引用防止崩溃
	//Create a WeakPtr to prevent crash
	TWeakObjectPtr<UFRPSubsystem> weak_this(this);

	//在线程池里使用一个线程执行
	//Execute in thread pool
	Async(EAsyncExecution::ThreadPool, [weak_this, send_msg]()
	{
		//临时变量
		//Temporary variables
		int status = TCP_Unknown;
		FString recv_msg;

		ISocketSubsystem* socket_subsystem = nullptr;
		FSocket* socket = nullptr;

		do
		{
			//判断WeakPtr引用是否合法，如果不合法则返回
			//Check if WeakPtr is valid, return if not
			if (!weak_this.IsValid())
			{
				status = TCP_Unknown;
				break;
			}

			//获取SocketSubsystem，如果获取失败则返回
			//Get SocketSubsystem, return if failed
			socket_subsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
			if (!socket_subsystem)
			{
				status = TCP_SocketError;
				break;
			}

			//创建TCPSocket，如果创建失败则返回
			//Create TCP socket, return if failed
			socket = socket_subsystem->CreateSocket(NAME_Stream, TEXT("TCPSocket"), false);
			if (!socket)
			{
				status = TCP_SocketError;
				break;
			}

			//设置BaseIP为ip_addr，设置失败则返回
			//Parse BaseIP into ip_addr, return if failed
			FIPv4Address ip_addr;
			if (!FIPv4Address::Parse(weak_this->BaseIP, ip_addr))
			{
				status = TCP_ConnectError;
				break;
			}

			//绑设置到addr并且连接，连接失败则返回
			//Bind to addr and connect, return if failed
			TSharedRef<FInternetAddr> internet_addr = socket_subsystem->CreateInternetAddr();
			internet_addr->SetIp(ip_addr.Value);
			internet_addr->SetPort(weak_this->MsgPort);
			if (!socket->Connect(*internet_addr))
			{
				status = TCP_ConnectError;
				break;
			}

			//发送消息，失败则返回
			//Send message, return if failed
			FTCHARToUTF8 utf8_convert(*send_msg);
			int32 sent_bytes = 0;
			if (!socket->Send((uint8*)utf8_convert.Get(), utf8_convert.Length(), sent_bytes) || sent_bytes <= 0)
			{
				status = TCP_SendError;
				break;
			}

			//循环接收4字节头部大小，失败则返回
			//Receive 4-byte header size, return if failed
			uint32 msg_len = 0;
			{
				uint8* len_ptr = reinterpret_cast<uint8*>(&msg_len);
				int32 total_read = 0;
				while (total_read < 4)
				{
					int32 read_bytes = 0;
					if (!socket->Recv(len_ptr + total_read, 4 - total_read, read_bytes) || read_bytes <= 0)
					{
						status = TCP_RecvError;
						break;
					}
					total_read += read_bytes;
				}
				if (status != TCP_Unknown)
				{
					break;
				}
			}

			//如果接收到的大小=0或过大则返回
			//If received size is 0 or too large, return
			if (msg_len == 0 || msg_len > 1024 * 1024)
			{
				status = TCP_RecvError;
				break;
			}

			//按照大小接收消息
			//Receive message by size
			{
				//初始化buffer
				//Initialize buffer
				TArray<uint8> recv_buf;
				recv_buf.SetNumZeroed(msg_len + 1);

				//循环接收并累计接收大小
				//Receive in loop and accumulate size
				int32 total_read = 0;
				while (total_read < (int32)msg_len)
				{
					int32 read_bytes = 0;
					if (!socket->Recv(recv_buf.GetData() + total_read, msg_len - total_read, read_bytes) || read_bytes <= 0)
					{
						status = TCP_RecvError;
						break;
					}
					total_read += read_bytes;
				}

				//如果接收失败则返回
				//Return if receive failed
				if (status != TCP_Unknown)
				{
					break;
				}

				//将接收字节转为FString并返回成功
				//Convert received bytes to FString and return success
				recv_msg = FString(UTF8_TO_TCHAR((const char*)recv_buf.GetData()));
				status = TCP_Success;
			}
		}
		while (false);

		//关闭Socket
		//Close socket
		if (socket && socket_subsystem)
		{
			socket->Close();
			socket_subsystem->DestroySocket(socket);
			socket = nullptr;
		}

		//切回主线程执行OnMsgRecv
		//Switch back to game thread and call OnMsgRecv
		FString back_msg = FString(recv_msg);
		AsyncTask(ENamedThreads::GameThread, [weak_this, back_msg, status]()
		{
			if (weak_this.IsValid())
			{
				weak_this->OnMsgRecv(back_msg, status);
			}
		});
	});
}

void UFRPSubsystem::DoList(const FString& message, const TArray<FString>& param)
{
	//清空Session列表
	//Clear session list
	SessionList.Empty();

	//根据参数创建Session
	//Create sessions based on parameters
	for (const FString& str : param)
	{
		TArray<FString> item = UBaseLib::SplitString(str, '&');
		if (item.Num() == 3)
		{
			FFRPSession session;
			session.ID = item[0];
			session.UsePassword = item[1] == "true";
			session.Property = item[2];
			SessionList.Emplace(session);
		}
	}

	//调用委托
	//Call delegate
	if (OnSessionUpdate.IsBound())
	{
		OnSessionUpdate.Execute(SessionList);
	}
}

void UFRPSubsystem::DoAddr(const FString& message, const TArray<FString>& param)
{
	//如果参数!=2则抛弃
	//Discard if param count != 2
	if (param.Num() != 2)
	{
		return;
	}

	//调用委托
	//Call delegate
	if (OnAddrUpdate.IsBound())
	{
		const bool suc = param[1] != "error";
		OnAddrUpdate.Execute(suc, param[1]);
	}
}
