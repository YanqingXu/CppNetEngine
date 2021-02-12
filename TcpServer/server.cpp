#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<windows.h>
#include<WinSock2.h>
#include <iostream>
//#pragma comment(lib, "ws2_32.lib")	I have added it to project dependencies

struct DataPackage
{
	int age;
	char name[32];
};

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
		// 5 receive request from client
		int nLen = recv(_cSock, _recvBuf, 128, 0);
		if (nLen <= 0)
		{
			std::cout << "log info: client exited and task finished..." << std::endl;
			break;
		}
		std::cout << "log info: get command£º" << _recvBuf << std::endl;
		// 6 handle request
		if (0 == strcmp(_recvBuf, "getInfo"))
		{
			DataPackage dp = { 28,"YanqingXu" };
			send(_cSock, (const char*)&dp, sizeof(DataPackage), 0);
		}
		else {
			char msgBuf[] = "???.";
			send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
	}
	// 6 close socket
	closesocket(_sock);
	//---------------------end ---------------------

	//clean windows socket environment
	WSACleanup();
	return 0;
}