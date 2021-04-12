#include "TcpClient.h"

void TcpClient::InitSocket()
{
#ifdef _WIN32
	//start windows socket2.x environment
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
#endif // _WIN32
	if (INVALID_SOCKET != _sock)
	{
		printf("<socket=%d>πÿ±’æ…¡¨Ω”...\n", _sock);
		std::cout << "<socket=" << _sock << ">close old connection...";
		std::cout << std::endl;
		Close();
	}

	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
	{
		std::cout << "log info: creating socket failed..." << std::endl;
	}
	else {
		std::cout << "log info: creating socket successfully..." << std::endl;
	}
}

int TcpClient::Connect(const char* ip, unsigned short port)
{
	if (INVALID_SOCKET == _sock)
	{
		InitSocket();
	}

	// 2 connect to server
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else 
	_sin.sin_addr.s_addr = inet_addr(ip);
#endif
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret)
	{
		std::cout << "log info: connecting to server failed..." << std::endl;
	}
	else
	{
		std::cout << "log info: connecting to server successfully..." << std::endl;
	}
	return ret;
}

void TcpClient::Close()
{
	if (_sock != INVALID_SOCKET)
	{
#ifdef _WIN32
		closesocket(_sock);
		//clean windows socket environment
		WSACleanup();
#else
		close(_sock);
#endif
		_sock = INVALID_SOCKET;
	}
}

bool TcpClient::OnRun()
{
	if (!IsRun())
	{
		return false;
	}

	fd_set fdReads;
	FD_ZERO(&fdReads);
	FD_SET(_sock, &fdReads);
	timeval t = { 1,0 };
	int ret = select(_sock + 1, &fdReads, 0, 0, &t);
	if (ret < 0)
	{
		std::cout << "<socket=" << _sock << ">select task finished 1" << std::endl;
		return false;
	}
	if (FD_ISSET(_sock, &fdReads))
	{
		FD_CLR(_sock, &fdReads);

		if (-1 == RecvData(_sock))
		{
			std::cout << "<socket=" << _sock << ">select task finished 2" << std::endl;
			return false;
		}
	}
	return true;
}

bool TcpClient::IsRun()
{
	return _sock != INVALID_SOCKET;
}

int TcpClient::RecvData(SOCKET _cSock)
{
	//buffer
	char szRecv[4096] = {};
	// 5 receive data from server
	int nLen = (int)recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		std::cout << "failed to connect to server, task finished";
		std::cout << std::endl;
		return -1;
	}
	recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);

	OnNetMsg(header);
	return 0;
}

void TcpClient::OnNetMsg(DataHeader* header)
{
	switch (header->cmd)
	{
	case CMD::CMD_LOGIN_RESULT:
	{
		LoginResult* pLoginRes = (LoginResult*)header;
		std::cout << "receive data from server: CMD_LOGIN_RESULT" << std::endl;
		std::cout << "data length: " << pLoginRes->dataLength << std::endl;
	}
	break;
	case CMD::CMD_LOGOUT_RESULT:
	{
		LogoutResult* pLogoutRes = (LogoutResult*)header;
		std::cout << "receive data from server: CMD_LOGOUT_RESULT" << std::endl;
		std::cout << "data length: " << pLogoutRes->dataLength << std::endl;
	}
	break;
	case CMD::CMD_NEW_USER_JOIN:
	{
		NewUserJoin* pUserJoin = (NewUserJoin*)header;
		std::cout << "receive data from server: CMD_NEW_USER_JOIN" << std::endl;
		std::cout << "data length: " << pUserJoin->dataLength << std::endl;
	}
	break;
	}
}

int TcpClient::SendData(DataHeader* header)
{
	if (IsRun() && header)
	{
		return send(_sock, (const char*)header, header->dataLength, 0);
	}
	return SOCKET_ERROR;
}