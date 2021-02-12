#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<windows.h>
#include<WinSock2.h>
#include <iostream>
#include "package.h"

//#pragma comment(lib,"ws2_32.lib")

int main()
{
	//start windows socket2.x environment
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);

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
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret)
	{
		std::cout<<"log info: connecting to server failed..."<<std::endl;
	}
	else
	{
		std::cout << "log info: connecting to server successfully..." << std::endl;
	}

	while (true)
	{
		//3. input request command
		char cmdBuf[128] = {};
		std::cin >> cmdBuf;
		//4 handle request command
		if (0 == strcmp(cmdBuf, "exit"))
		{
			std::cout<<"get exit command, task finished";
			break;
		}
		else if (0 == strcmp(cmdBuf, "login")) {
			Login login = { "YanqingXu","123456" };
			DataHeader dh = { sizeof(login), CMD::CMD_LOGIN };
			//5 send request command to server
			send(_sock, (const char*)&dh, sizeof(dh), 0);
			send(_sock, (const char*)&login, sizeof(login), 0);
			// receive data from server
			DataHeader retHeader = {};
			LoginResult loginRet = {};
			recv(_sock, (char*)&retHeader, sizeof(retHeader), 0);
			recv(_sock, (char*)&loginRet, sizeof(loginRet), 0);
			std::cout << "log info: LoginResult = " << loginRet.result << std::endl;
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			Logout logout = { "YanqingXu" };
			DataHeader dh = { sizeof(logout), CMD::CMD_LOGOUT };
			send(_sock, (const char*)&dh, sizeof(dh), 0);
			send(_sock, (const char*)&logout, sizeof(logout), 0);
			DataHeader retHeader = {};
			LogoutResult logoutRet = {};
			recv(_sock, (char*)&retHeader, sizeof(retHeader), 0);
			recv(_sock, (char*)&logoutRet, sizeof(logoutRet), 0);
			std::cout << "log info: LogoutResult = " << logoutRet.result << std::endl;
		}
		else 
		{
			std::cout << "log info: wrong command" << std::endl;
		}
	}
	// 7 close socket
	closesocket(_sock);
	//---------------------end ---------------------

	//clean windows socket environment
	WSACleanup();
	std::cout<<"log info: client exit";
	getchar();
	return 0;
}