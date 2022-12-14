#define _WINSOCK_DEPRECATED_NO_WARNINGS
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
#include<vector>

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


///////////////global variable////////////////
char recvBuf[128] = "";
char recvBufCopy[128] = "";
int err = 1;
struct Login login;
struct Logout logout;
struct Loginresult loginresult;
struct Logoutresult logoutresult;
char name[8] = "hooyoog";
char pass[8] = "aaaa1234";

std::vector<SOCKET> all_Client = {};

fd_set read_Client = {};
fd_set write_Client = {};
fd_set new_Client = {};


//////////////////Global function//////////////////////
void initDB() {
	memcpy(login.name, name, sizeof(name));
	memcpy(login.pass, pass, sizeof(pass));
	memcpy(logout.name, name, sizeof(name));
}

SOCKET initListenSocket() {
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	SOCKET listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(14567);

#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");
#else
	_sin.sin_addr.s_addr = INADDR_ANY;//inet_addr("127.0.0.1");
#endif

	if (SOCKET_ERROR == bind(listenfd, (sockaddr*)&_sin, sizeof(_sin))) {
		printf("bing error");
	}

	if (SOCKET_ERROR == listen(listenfd, 5)) {
		printf("error\n");
	}
	else {
		printf("listen is ready!\n");
	}
	return listenfd;
}



SOCKET acceptGetClient(SOCKET listenfd) {

	SOCKET _cSock = INVALID_SOCKET;
	sockaddr_in cliAddr = {};
#ifdef _WIN32
	int nAddrlen = sizeof(sockaddr_in);
#else
	socklen_t nAddrlen = sizeof(sockaddr_in);
#endif
	_cSock = accept(listenfd, (sockaddr*)&cliAddr, &nAddrlen);

	if (_cSock == INVALID_SOCKET) {
		printf("accept error\n");
	}
	else {
		printf("socket: %d , ip :%s connect\n", _cSock, inet_ntoa(cliAddr.sin_addr));
	}
	return _cSock;
}



int doSomeThing(SOCKET _cSock) {
	printf("do ...\n");
	memset(recvBuf, 0, sizeof(recvBuf));
	memset(recvBufCopy, 0, sizeof(recvBufCopy));
	err = recv(_cSock, recvBuf, sizeof(Head), 0);
	if (err <= 0) {
		printf("break by :  %d\n", _cSock);

#ifdef _WIN32
		closesocket(_cSock);
#else
		close(_cSock);
#endif
		return -1;
	}

	Head* head = ((Head*)&recvBuf);
	int cmd = ((Head*)&recvBuf)->cmd;
	int lens = ((Head*)&recvBuf)->msgLens;
	printf("getCMD :  %d\n", cmd);

	switch (cmd) {
	case LOGIN:
	{
		int x = recv(_cSock, recvBuf + sizeof(Head), head->msgLens - sizeof(Head), 0);
		printf("recv user login id:%d\n", _cSock);
		memcpy(recvBufCopy, recvBuf, head->msgLens);

		if (strcmp(((Login*)recvBufCopy)->name, login.name) == 0 || strcmp(((Login*)recvBufCopy)->pass, login.pass) == 0) {
			printf("shijin login id:%d\n", _cSock);

			memset(recvBuf, 0, sizeof(recvBuf));
		
			send(_cSock, (const char*)&loginresult, sizeof(loginresult), 0);

#if _DEBUG
			printf("DEBUG------->send %d\n", loginresult.result);
#endif
		}
		printf("-----------------------------------------\n");
		memset(recvBufCopy, 0, sizeof(recvBufCopy));
	}break;

	case LOGOUT:
	{
		int x = recv(_cSock, recvBuf + sizeof(Head), head->msgLens - sizeof(Head), 0);
		printf("user logout\n");

		memcpy(recvBufCopy, recvBuf, head->msgLens);
		if (strcmp(((Logout*)recvBufCopy)->name, logout.name) == 0) {
			printf("shijin logout\n");


			//Sleep(1);
			send(_cSock, (const char*)&logoutresult, sizeof(logoutresult), 0);
#if _DEBUG
			printf("DEBUG------->send %d\n", logoutresult.result);
#endif
		}
		printf("-----------------------------------------\n");
		memset(recvBufCopy, 0, sizeof(recvBufCopy));
	}break;

	default: {

		printf("???? %d\n", _cSock);
	}


	}//switch

	return 0;

}

int newUserLoginBroad(SOCKET _cSock) {

	New_user_login new_user_login;
	for ( int i = (int)all_Client.size()-1; i >=0 ; i--) {
			
		send(all_Client[i], (const char*)&new_user_login, sizeof(new_user_login), 0);	

	}
	printf("socket:%d broad\n", _cSock);

	return 0;
}


int main() {
	SOCKET listenfd = initListenSocket();

	printf("------------------------------------\n");

	initDB();
	timeval tm;
	tm.tv_sec = 0;
	tm.tv_usec = 0;

	while (1) {

		FD_SET(listenfd, &new_Client);

		int ret = select(listenfd + 1, &new_Client, NULL, NULL, &tm);
		if (ret == -1) {
			continue;
		}
		else {
			if (FD_ISSET(listenfd, &new_Client)) {
				SOCKET _cSock;
				_cSock = acceptGetClient(listenfd);
				newUserLoginBroad(_cSock);
				all_Client.push_back(_cSock);
				printf("intert socket = %d\n",(int)all_Client[all_Client.size()-1]);

			}
		}


		FD_ZERO(&read_Client);
		SOCKET max_fd = listenfd;
		for (int i = (int)all_Client.size()-1; i >=0; i--) {
				FD_SET(all_Client[i], &read_Client);
				
				if (max_fd < all_Client[i]) {
					max_fd = all_Client[i];
				}
		}

		
		int ret2 = select(max_fd+1, &read_Client, NULL, NULL, &tm);
		if (ret2 > 0) {
#ifdef _WIN32
			//just used for windows
			for (int i = 0; i < (int)read_Client.fd_count; i++) {
			int err = doSomeThing(read_Client.fd_array[i]);
				if (err == -1) {
					auto iterator = std::find(all_Client.begin(), all_Client.end(), read_Client.fd_array[i]);
					if (iterator != all_Client.end()) {
						all_Client.erase(iterator);
					}
				}			
			}
#else
			//both win32 and linux
			for (int i = (int)all_Client.size() - 1; i >= 0; i--) {
				if (FD_ISSET(all_Client[i], &read_Client)) {
					int err = doSomeThing(all_Client[i]);
					if (err == -1) {
						auto iterator = all_Client.begin()+i;
						if (iterator != all_Client.end()) {
							all_Client.erase(iterator);
						}
					}
				}
			}
#endif

		}
#ifdef _WIN32
		//Sleep(2000);
#else
		//sleep(2);
#endif
	}

#ifdef _WIN32
	for (int n = all_Client.size() - 1; n >= 0; n--) {
		closesocket(all_Client[n]);
	}

	closesocket(listenfd);
	WSACleanup();
#else
	for (int n = all_Client.size() - 1; n >= 0; n--) {
		printf("close all \n");
		close(all_Client[n]);
}

	close(listenfd);
#endif
	getchar();

	return 0;
}
