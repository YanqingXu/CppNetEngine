#include "TcpClient.h"

void cmdThread(TcpClient* pClient)
{
	while (true)
	{
		char cmdBuf[256] = {};
		std::cin >> cmdBuf;
		if (0 == strcmp(cmdBuf, "exit"))
		{
			pClient->Close();
			std::cout << "exit cmdThread!" << std::endl;
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy(login.userName, "XuYanqing");
			strcpy(login.password, "SnnyRain");
			pClient->SendData(&login);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			Logout logout;
			strcpy(logout.userName, "XuYanqing");
			pClient->SendData(&logout);
		}
		else {
			std::cout << "ERROR CMD!!!" << std::endl;
		}
	}
}

bool g_bRun = true;

int main()
{
	TcpClient client;
	client.Connect(std::string("127.0.0.1").c_str(), 4567);

	//launch a thread
	std::thread t1(cmdThread, &client);
	t1.detach();

	while (client.IsRun())
	{
		client.OnRun();
	}

	client.Close();
	std::cout << "have exited..." << std::endl;

	getchar();
	return 0;
}