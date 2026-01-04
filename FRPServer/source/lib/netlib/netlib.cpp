#include "netlib.h"

#include "lib/baselib/baselib.h"

void netlib::init_wsadata(WSADATA& wsa_data)
{
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
	{
		baselib::error(TEXT("WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0"), TEXT("netlib::init_wsadata"));
	}
}

SOCKET netlib::make_socket(const int& port, const bool& block)
{
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		baselib::error(TEXT("sock == INVALID_SOCKET"), TEXT("netlib::make_socket"));
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (bind(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		baselib::error(TEXT("bind(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR"), TEXT("netlib::make_socket"));
	}

	u_long mode = block ? 0 : 1;
	ioctlsocket(sock, FIONBIO, &mode);

	return sock;
}

sockaddr_in netlib::make_addr(const char* ip, const int& port)
{
	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);
	return addr;
}

std::string netlib::get_ip(const sockaddr_in& addr)
{
	char buf[INET_ADDRSTRLEN] = { 0 };
	inet_ntop(AF_INET, &(addr.sin_addr), buf, INET_ADDRSTRLEN);
	return std::string(buf);
}

std::string netlib::get_ip(const SOCKET& sock)
{
	return get_ip(get_addr(sock));
}

int netlib::get_port(const sockaddr_in& addr)
{
	return ntohs(addr.sin_port);
}

int netlib::get_port(const SOCKET& sock)
{
	return get_port(get_addr(sock));
}

sockaddr_in netlib::get_addr(const SOCKET& sock)
{
	sockaddr_in addr{};
	socklen_t len = sizeof(addr);
	getsockname(sock, (sockaddr*)&addr, &len);
	return addr;
}

bool netlib::equal(const sockaddr_in& addr1, const sockaddr_in& addr2)
{
	return get_ip(addr1) == get_ip(addr2) && get_port(addr1) == get_port(addr2);
}

bool netlib::recv(const SOCKET& sock, char* buffer, int buffer_len, sockaddr_in& addr, int& recvlen)
{
	socklen_t addr_size = sizeof(addr);
	recvlen = recvfrom(sock, buffer, buffer_len, 0, (sockaddr*)&addr, &addr_size);
	return recvlen > 0;
}

void netlib::send(const SOCKET& sock, const sockaddr_in& addr, const char* data, int data_len)
{
	sendto(sock, data, data_len, 0, (const sockaddr*)&addr, sizeof(addr));
}

void netlib::send(const SOCKET& sock, const sockaddr_in& addr, const char* add, const char* data, int data_len)
{
	char sendbuf[2048];

	int add_len = (int)strlen(add);
	if (add_len + data_len >= (int)sizeof(sendbuf))
	{
		printf("netlib::send add too long!\n");
		return;
	}

	memcpy(sendbuf, add, add_len);
	memcpy(sendbuf + add_len, data, data_len);

	sendto(sock, sendbuf, add_len + data_len, 0, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
}
