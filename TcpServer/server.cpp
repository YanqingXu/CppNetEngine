#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<windows.h>
#include<WinSock2.h>
#include <iostream>
#include <string>
#include <vector>
#include "package.h"
//#pragma comment(lib, "ws2_32.lib")	I have added it to project dependencies


std::vector<SOCKET> g_clients;

int processor(SOCKET _cSock)
{
	//buffer
	char szRecv[4096] = {};
	// 5 receive data from client
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	if (nLen <= 0)
	{
		std::cout << "log info: client exited and task finished." << std::endl;
		return -1;
	}
	DataHeader* header = reinterpret_cast<DataHeader*>(szRecv);
	switch (header->cmd)
	{
	case CMD::CMD_LOGIN:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		Login* login = reinterpret_cast<Login*>(szRecv);
		std::cout << "receive command: CMD_LOGIN  ";
		std::cout << "data length = " << login->dataLength << std::endl;
		std::cout << "data info: (userName=" << login->userName << ", password=" << login->password << ")";
		std::cout << std::endl;
		LoginResult ret;
		send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
	}
	break;
	case CMD::CMD_LOGOUT:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		Logout* logout = reinterpret_cast<Logout*>(szRecv);
		std::cout << "receive command: CMD_LOGOUT  ";
		std::cout << "data length = " << logout->dataLength << std::endl;
		std::cout << "data info: (userName=" << logout->userName << ")";
		std::cout << std::endl;
		LogoutResult ret;
		send(_cSock, (char*)&ret, sizeof(ret), 0);
	}
	break;
	default:
	{
		DataHeader header = { 0,CMD::CMD_ERROR };
		send(_cSock, (char*)&header, sizeof(header), 0);
	}
	}
}

int main()
{
	//start windows socket2.x environment
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);

	//------------create a easy tcp server with socket API------------
	// 1 create a socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// 2 bind a port used to connect a client
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);				//Host TO Net Short(port)
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;		//ip("127.0.0.1");
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))
	{
		std::cout<<"log info: binding port 4567 failed..."<<std::endl;
	}
	else
	{
		std::cout << "log info: bind port 4567 successfully..." << std::endl;
	}
	// 3 listen port
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		std::cout << "log info: listen port 4567 failed..." << std::endl;
	}
	else
	{
		std::cout << "log info: listen port 4567 successfully..." << std::endl;
	}
	// 4 wait a client to connect it
	/*sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;
	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock)
	{
		std::cout << "log info: invalid socket..." << std::endl;
	}
	std::cout << "log info: a new client added£ºIP = " << inet_ntoa(clientAddr.sin_addr) << std::endl;*/

	while (true)
	{
		//Berkeley Socket
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;

		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);

		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExp);

		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			FD_SET(g_clients[n], &fdRead);
		}

		///nfds is an integer, which means the range of all socket from fd_set;
		///it equal to the biggest value of all socket plus 1

		timeval t = { 0,0 };
		int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0)
		{
			std::cout << "select finished" << std::endl;
			break;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			// 4 accept (wait to accept connection from client)
			sockaddr_in clientAddr = {};
			int nAddrLen = sizeof(sockaddr_in);
			SOCKET _cSock = INVALID_SOCKET;
			_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
			if (INVALID_SOCKET == _cSock)
			{
				std::cout << "ERROR, INVALID SOCKET..." << std::endl;
			}
			else
			{
				for (int n=(int)g_clients.size()-1; n>=0; n--)
				{
					NewUserJoin userJoin;
					send(g_clients[n], (const char*)&userJoin, sizeof(NewUserJoin), 0);
				}
				g_clients.push_back(_cSock);
				std::cout << "new client added!!!" << std::endl;
				std::cout << "socket = " << static_cast<int>(_cSock) << "  ";
				std::cout << "IP = " << inet_ntoa(clientAddr.sin_addr) << std::endl;
			}
		}
		for (size_t n = 0; n < fdRead.fd_count; n++)
		{
			if (-1 == processor(fdRead.fd_array[n]))
			{
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end())
				{
					g_clients.erase(iter);
				}
			}
		}
		Sleep(1000);
		std::cout << "handle other things..." << std::endl;
	}
	for (size_t n = g_clients.size() - 1; n >= 0; n--)
	{
		closesocket(g_clients[n]);
	}

	// 5 close socket
	closesocket(_sock);
	//---------------------end ---------------------

	//clean windows socket environment
	WSACleanup();
	std::cout << "Server have exited!!!" << std::endl;
	getchar();
	return 0;
}