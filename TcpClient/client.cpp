#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<windows.h>
#include<WinSock2.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")

int main()
{
	//start windows socket2.x environment
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	//------------create a easy tcp client with socket API------------
	// 1 create a socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock)
	{
		printf("log info: creating socket failed...\n");
	}
	else
	{
		printf("log info: creating socket successfully...\n");
	}
	// 2 connect to server
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret)
	{
		printf("log info: connecting to server failed...\n");
	}
	else
	{
		printf("log info: connecting to server successfully...\n");
	}
	// 3 receive data from server
	char recvBuf[256] = {};
	int nlen = recv(_sock, recvBuf, 256, 0);
	if (nlen > 0)
	{
		printf("receive data£º%s \n", recvBuf);
	}
	// 4 close socket
	closesocket(_sock);
	//---------------------end ---------------------

	//clean windows socket environment
	WSACleanup();
	getchar();
	return 0;
}