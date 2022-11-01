#ifndef _EasyTcpClient_hpp_
#define _EasyTcpClient_hpp_
#ifdef _WIN32
#include<WinSock2.h>
#include<windows.h>
#pragma comment(lib,"ws2_32.lib")

#else
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR           (-1)
#endif

#include<stdio.h>
#include<string.h>
#include<thread>

#include "MessageHead.hpp"

class EasyTcpClient
{
	SOCKET _server_sock;
public:
	EasyTcpClient()
	{
		_server_sock = INVALID_SOCKET;


	}

	virtual ~EasyTcpClient()
	{
		Close();
	}

	//initSocket
	int InitSocket()
	{
#ifdef _WIN32
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif
		if (INVALID_SOCKET != _server_sock) {
			printf("<socket = %d> Old Connect broke!\n", _server_sock);
			Close();
		}
		_server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (INVALID_SOCKET == _server_sock)
		{
			printf("error , socket broken...\n");
		}
		else {
			printf("socket connect secces!!!\n");
		}
		return 0;
	}

	//connect
	int Connect(const char* ip, unsigned short port)
	{
		if (INVALID_SOCKET == _server_sock) {
			InitSocket();
		}
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
#ifdef _WIN32	
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);//inet_addr("169.254.122.10");
#else

		_sin.sin_addr.s_addr = inet_addr(ip);//inet_addr("127.0.0.1");
#endif
		int err = connect(_server_sock, (sockaddr*)&_sin, sizeof(_sin));
		if (err == SOCKET_ERROR) {
			printf("connect error!!!  <socket id = %d> - <%s:%d>\n", _server_sock, ip, port);
		}
		else {
			printf("connect <socket id = %d> - <%s:%d>\n", _server_sock,ip,port);
		}
		return err;
	}

	void Close()
	{
		if (_server_sock != INVALID_SOCKET) {
#ifdef _WIN32
			closesocket(_server_sock);
			WSACleanup();
#else
			close(_server_sock);
#endif
			_server_sock = INVALID_SOCKET;
		}
		

	}

	

	//OnRun
	bool OnRun()
	{
		if (isRun())
		{
			fd_set fdReads;
			FD_ZERO(&fdReads);
			FD_SET(_server_sock, &fdReads);
			timeval t = { 0 ,0 };
			int ret = select(_server_sock + 1, &fdReads, 0, 0, &t);
			if (ret < 0)
			{
				printf("<socket = %d > select over.1 \n", _server_sock);
				Close();
				return false;
			}
			if (FD_ISSET(_server_sock, &fdReads))
			{
				FD_CLR(_server_sock, &fdReads);
				if (-1 == RecvData(_server_sock))
				{
					printf("<socket = %d > select over.2 \n", _server_sock);
					Close();
					return false;
				}
			}
			return true;
		}
		return false;
	}

	bool isRun()
	{
		return _server_sock != INVALID_SOCKET;
	}

	///////////processor function/////////////
	//处理粘包、拆分
	int RecvData(SOCKET _socket) {
		char recvBuf[4096] = {};

		int x = recv(_socket, recvBuf, sizeof(Head), 0);//real recv
		if (x <= 0) {
			printf("brocken connect server..\n");
			return -1;
		}
		printf("recv lens<---------------%d Bytes\n", x);//lens
		Head* head = ((Head*)(&recvBuf));

		int y = recv(_socket, recvBuf + sizeof(Head), head->msgLens - sizeof(Head), 0);
		if (y <= 0) { printf("recv\n"); return -1; }
		OnNetMsg(head);
		return 0;
	}

	void OnNetMsg(Head* head)
	{
		switch (head->cmd)
		{
		case LOGINRESULT:
		{
			Loginresult* logres = ((Loginresult*)head);//扩充成大的
			printf("<socket = %d> recv Login result , lens: %d\n",_server_sock, logres->msgLens);
		}
		break;
		case LOGOUTRESULT:
		{
			Logoutresult* logres = ((Logoutresult*)head);
			printf("<socket = %d> recv Logout result , lens: %d\n", _server_sock,logres->msgLens);
		}
		break;

		case NEW_USER_LOGIN:
		{
			New_user_login* logres = ((New_user_login*)head);
			printf("<socket = %d> recv new Login result ， lens: %d\n", _server_sock,logres->msgLens);
		}
		break;
		default:
		{
			printf("recv ?????");
		}break;


		}//switch
		printf("------------------------------------\n");
	}

	//send
	int SendData(Head* head)
	{
		if (isRun() && head) {
			return send(_server_sock, (const char*)head, head->msgLens, 0);
		}
		return SOCKET_ERROR;
	}

private:

};

#endif // !_EasyTcpClient_hpp_

