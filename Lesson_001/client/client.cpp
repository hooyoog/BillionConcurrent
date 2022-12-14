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


enum CMD {
	LOGIN,
	LOGINRESULT,
	LOGOUT,
	LOGOUTRESULT,
	NEW_USER_LOGIN
};

struct Head {
	short msgLens;
	short cmd;
};
struct Login :public Head {
	Login() {
		msgLens = sizeof(Login);
		cmd = LOGIN;
	}
	char name[8];
	char pass[8];
};
struct Loginresult :public Head {
	Loginresult() {
		msgLens = sizeof(Loginresult);
		cmd = LOGINRESULT;
		result = 200;
	}
	int result;
};
struct Logout :public Head {
	Logout() {
		msgLens = sizeof(Logout);
		cmd = LOGOUT;
	}
	char name[8];
};
struct Logoutresult :public Head {
	Logoutresult() {
		msgLens = sizeof(Logoutresult);
		cmd = LOGOUTRESULT;
		result = 202;
	}
	int result;
};

struct New_user_login :public Head {

	New_user_login() {
		msgLens = sizeof(New_user_login);
		cmd = NEW_USER_LOGIN;
		result = 206;
	}
	int result;
};




char recvBuf[128] = "";
char recvBufCopy[128] = "";
fd_set readfd = {};
timeval tm = { 3,0 };
struct Login login;
struct Logout logout;


///////////Global function/////////////
void recvMessage(SOCKET _socket) {
	printf("socket id %d\n", _socket);//socketid
	memset(recvBuf, 0, 128);//clean buffer
	memset(recvBufCopy, 0, 128);

	int x = recv(_socket, recvBuf, sizeof(Head), 0);//real recv
	printf("recv lens<---------------%d Bytes\n", x);//lens

	Head* head = ((Head*)(&recvBuf));
	switch (head->cmd)
	{
	case LOGINRESULT:
	{
		int x = recv(_socket, recvBuf + sizeof(Head), head->msgLens - sizeof(Head), 0);
		if (x <= 0) { printf("recv\n"); break; }
		Loginresult* logres = ((Loginresult*)(&recvBuf));
		printf("recvBuf result: %d\n", logres->result);
		memcpy((Loginresult*)recvBufCopy, recvBuf, sizeof(Loginresult));
		memset(recvBuf, 0, sizeof(recvBuf));

#if _DEBUG
		printf("DEBUG logres->cmd ------->recv cmd %d\n", logres->cmd);
#endif
		if (sizeof(recvBufCopy) >= 1) {

			Loginresult* h = (Loginresult*)&recvBufCopy;


			if (h->cmd == LOGINRESULT) {
				printf("copy result: %d\n", h->result);
				memset(recvBufCopy, 0, sizeof(recvBufCopy));
			}
		}
		
	}break;
	case LOGOUTRESULT:
	{
		int x = recv(_socket, recvBuf + sizeof(Head), head->msgLens - sizeof(Head), 0);
		if (x <= 0) { printf("recv \n"); break; }
		Logoutresult* logres = ((Logoutresult*)(&recvBuf));
		printf("recvBuf result: %d\n", logres->result);
		memcpy((Logoutresult*)recvBufCopy, recvBuf, sizeof(Logoutresult));
		memset(recvBuf, 0, sizeof(recvBuf));

#if _DEBUG
		printf("DEBUG logres->cmd------->recv cmd %d\n", logres->cmd);
#endif
		if (sizeof(recvBufCopy) >= 1) {

			Logoutresult* h = (Logoutresult*)&recvBufCopy;


			if (h->cmd == LOGOUTRESULT) {
				printf("copy result: %d\n", h->result);
				memset(recvBufCopy, 0, sizeof(recvBufCopy));
			}
		}
		
	}break;

	case NEW_USER_LOGIN:
	{
		int x = recv(_socket, recvBuf + sizeof(Head), head->msgLens - sizeof(Head), 0);
		if (x <= 0) { printf("recv\n"); break; }
		New_user_login* logres = ((New_user_login*)(&recvBuf));
		printf("recvBuf result: %d\n", logres->result);
		memcpy((New_user_login*)recvBufCopy, recvBuf, sizeof(New_user_login));
		memset(recvBuf, 0, sizeof(recvBuf));

#if _DEBUG
		printf("DEBUG  logres->cmd------->recv cmd %d\n", logres->cmd);
#endif
		if (sizeof(recvBufCopy) >= 1) {

			New_user_login* h = (New_user_login*)&recvBufCopy;


			if (h->cmd == NEW_USER_LOGIN) {
				printf("copy result: %d\n", h->result);
				memset(recvBufCopy, 0, sizeof(recvBufCopy));
			}
		}
		
	}break;
	default:
	{
		printf("recv ?????");
	}break;


	}//switch


	printf("------------------------------------\n");

}


bool is_Run = true;
void cmdThread(SOCKET server) {
	char name[8] = "hooyoog";
	char pass[8] = "aaaa1234";
	memcpy(login.name, name, sizeof(name));
	memcpy(login.pass, pass, sizeof(pass));

	memcpy(logout.name, name, sizeof(name));
	
	while (1) {
		char msgBuf[8] = {};
		
		scanf("%s", msgBuf);

		if (strcmp(msgBuf, "exit") == 0)
		{
#ifdef _WIN32
			closesocket(server);
#else

			close(server);
#endif
			is_Run = false;
			break;
		}
		else if (strcmp(msgBuf, "Login") == 0)
		{
			printf("Login >>\n");

			int err = send(server, (const char*)&login, sizeof(login), 0);
			if (err > 1) { printf("send finish>>>\n"); }
			

		}
		else if (strcmp(msgBuf, "Logout") == 0)
		{
			printf("Logout >>\n");
			int err = send(server, (const char*)&logout, sizeof(logout), 0);
			if (err > 1) { printf("send finish>>>\n"); }
			

		}
		else {
			continue;
		}
		Sleep(2000);
	}
}

int main() {

#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif

	SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(14567);
#ifdef _WIN32	
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//inet_addr("169.254.122.10");
#else
	
	_sin.sin_addr.s_addr = inet_addr("169.254.122.1");//inet_addr("127.0.0.1");
#endif
	int err = connect(server, (sockaddr*)&_sin, sizeof(_sin));
	if (err == SOCKET_ERROR) {
		printf("connect error\n");
	}
	else {
		printf("socket id %d\n", server);

		std::thread t(cmdThread, server);
		t.detach();
		int num = 0;
		
		while (is_Run)
		{
			//printf("------------------while(%d)------------------\n", num++);
			FD_SET(server, &readfd);
			select(server+1, &readfd, NULL, NULL, &tm);
			//Sleep(800);
			if (FD_ISSET(server, &readfd)) {
				recvMessage(server);
				memset(recvBuf, 0, sizeof(recvBuf));
			}

		}

	}
	getchar();//for don"t close
#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}
