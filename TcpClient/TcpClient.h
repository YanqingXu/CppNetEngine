#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include<WinSock2.h>
//#pragma comment(lib,"ws2_32.lib")

#else
#include<unistd.h>
#include<arpa/inet.h>

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)
#endif

#include <iostream>
#include <thread>
#include "package.h"

class TcpClient
{
public:
	TcpClient(): _sock(INVALID_SOCKET)
	{

	}

	~TcpClient()
	{

	}

	void InitSocket();

	int Connect(const char* ip, unsigned short port);

	void Close();

	bool OnRun();

	bool IsRun();

	int RecvData(SOCKET _cSock);

	void OnNetMsg(DataHeader* header);

	int SendData(DataHeader* header);

private:
	SOCKET _sock;
};
#endif // !_TCP_CLIENT_H_
