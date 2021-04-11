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



int processor(SOCKET _cSock)
{
	//buffer
	char szRecv[4096] = {};
	// 5 receive data from server
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	if (nLen <= 0)
	{
		std::cout << "failed to connect to server, task finished";
		std::cout << std::endl;
		return -1;
	}
	DataHeader* header = (DataHeader*)szRecv;
	switch (header->cmd)
	{
	case CMD::CMD_LOGIN_RESULT:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LoginResult* pLoginRes = (LoginResult*)szRecv;
		std::cout << "receive data from server: CMD_LOGIN_RESULT" << std::endl;
		std::cout << "data length: " << pLoginRes->dataLength << std::endl;
	}
	break;
	case CMD::CMD_LOGOUT_RESULT:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LogoutResult* pLogoutRes = (LogoutResult*)szRecv;
		std::cout << "receive data from server: CMD_LOGOUT_RESULT" << std::endl;
		std::cout << "data length: " << pLogoutRes->dataLength << std::endl;
	}
	break;
	case CMD::CMD_NEW_USER_JOIN:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		NewUserJoin* pUserJoin = (NewUserJoin*)szRecv;
		std::cout << "receive data from server: CMD_NEW_USER_JOIN" << std::endl;
		std::cout << "data length: " << pUserJoin->dataLength << std::endl;
	}
	break;
	}
	return 0;
}

bool g_bRun = true;
void cmdThread(SOCKET sock)
{
	while (true)
	{
		char cmdBuf[256] = {};
		std::cin >> cmdBuf;
		if (0 == strcmp(cmdBuf, "exit"))
		{
			g_bRun = false;
			std::cout << "exit cmdThread!" << std::endl;
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy(login.userName, "XuYanqing");
			strcpy(login.password, "SnnyRain");
			send(sock, (const char*)&login, sizeof(Login), 0);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			Logout logout;
			strcpy(logout.userName, "XuYanqing");
			send(sock, (const char*)&logout, sizeof(Logout), 0);
		}
		else {
			std::cout << "ERROR CMD!!!" << std::endl;
		}
	}
}

int main()
{
#ifdef _WIN32
	//start windows socket2.x environment
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
#endif

	//------------create a easy tcp client with socket API------------
	// 1 create a socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock)
	{
		std::cout<<"log info: creating socket failed..."<<std::endl;
	}
	else
	{
		std::cout<<"log info: creating socket successfully..."<<std::endl;
	}
	// 2 connect to server
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
#else 
	_sin.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret)
	{
		std::cout<<"log info: connecting to server failed..."<<std::endl;
	}
	else
	{
		std::cout << "log info: connecting to server successfully..." << std::endl;
	}
	//launch a thread
	std::thread t(cmdThread, _sock);
	t.detach();

	while (g_bRun)
	{
		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(_sock, &fdReads);

		timeval t = { 0, 0 };
		int ret = select(_sock, &fdReads, 0, 0, &t);
		if (ret < 0)
		{
			std::cout << "select task finished";
			break;
		}
		if (FD_ISSET(_sock, &fdReads))
		{
			FD_CLR(_sock, &fdReads);
			if (-1 == processor(_sock))
			{
				std::cout << "select task finished" << std::endl;
				break;
			}
		}

		std::cout << "handle other things..." << std::endl;
		
		Login login;
		strcpy(login.userName, "XuYanqing");
		strcpy(login.password, "SnnyRain");
		send(_sock, (const char*)&login, sizeof(Login), 0);
		Sleep(1000);
	}
	// 7 close socket
	
	//---------------------end ---------------------
#ifdef _WIN32
	closesocket(_sock);
	//clean windows socket environment
	WSACleanup();
#else
	close(_sock);
#endif // _WIN32

	
	std::cout<<"log info: client exit";
	getchar();
	return 0;
}