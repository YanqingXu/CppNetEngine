#define WIN32_LEAN_AND_MEAN
#include<windows.h>
#include<WinSock2.h>
//#pragma comment(lib, "ws2_32.lib")	已手动添加到项目属性链接器里的依赖附加项里

int main()
{
	//启动windows socket2.x环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);


	//清除windows socket环境
	WSACleanup();
	return 0;
}