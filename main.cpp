#define WIN32_LEAN_AND_MEAN
#include<windows.h>
#include<WinSock2.h>
//#pragma comment(lib, "ws2_32.lib")	���ֶ���ӵ���Ŀ�������������������������

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);

	WSACleanup();
	return 0;
}