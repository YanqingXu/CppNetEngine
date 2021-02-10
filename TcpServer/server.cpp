#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<windows.h>
#include<WinSock2.h>
#include <stdio.h>
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
	_sin.sin_port = htons(4567);				//port
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;		//ip("127.0.0.1");
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))
	{
		printf("log info: binding port 4567 failed...\n");
	}
	else
	{
		printf("log info: bind port 4567 successfully...\n");
	}
	// 3 listen port
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		printf("log info: listen port 4567 failed...\n");
	}
	else
	{
		printf("log info: listen port 4567 successfully...\n");
	}
	// 4 wait a client to connect it
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;
	char msgBuf[] = "Hello, I'm Server.";

	while (true)
	{
		_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
		if (INVALID_SOCKET == _cSock)
		{
			printf("log info: invalid socket...\n");
		}
		printf("log info: a new client added£ºIP = %s \n", inet_ntoa(clientAddr.sin_addr));
		// 5 send data to client
		send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
	}
	// 6 close socket
	closesocket(_sock);
	//---------------------end ---------------------

	//clean windows socket environment
	WSACleanup();
	return 0;
}