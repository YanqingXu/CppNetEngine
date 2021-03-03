#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<windows.h>
#include<WinSock2.h>
#include <iostream>
#include <string>
#include "package.h"
//#pragma comment(lib, "ws2_32.lib")	I have added it to project dependencies

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
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;
	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock)
	{
		std::cout << "log info: invalid socket..." << std::endl;
	}
	std::cout << "log info: a new client added£ºIP = " << inet_ntoa(clientAddr.sin_addr) << std::endl;

	char _recvBuf[128] = {};

	while (true)
	{
		//buffer
		char szRecv[4096] = {};
		// 5 receive data from client
		int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			std::cout << "log info: client exited and task finished." << std::endl;
			break;
		}
		DataHeader* header = (DataHeader*)szRecv;
		switch (header->cmd)
		{
			case CMD::CMD_LOGIN:
			{
				Login* login = reinterpret_cast<Login*>(szRecv);
				recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
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
				Logout* logout = reinterpret_cast<Logout*>(szRecv);
				recv(_cSock, szRecv+sizeof(DataHeader), logout->dataLength - sizeof(DataHeader), 0);
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
	// 6 close socket
	closesocket(_sock);
	//---------------------end ---------------------

	//clean windows socket environment
	WSACleanup();
	return 0;
}