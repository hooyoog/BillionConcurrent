#include<string.h>
#include<thread>


#include "EasyTcpClient.hpp"



void cmdThread( EasyTcpClient* client) {
	
	char name[8] = "shijin";
	char pass[8] = "abc123";
	struct Login login;
	struct Logout logout;
	memcpy(login.name, name, sizeof(name));
	memcpy(login.pass, pass, sizeof(pass));
	memcpy(logout.name, name, sizeof(name));
	
	while (1) {
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (strcmp(cmdBuf, "exit") == 0)
		{
			client->Close();
			break;
		}
		else if (strcmp(cmdBuf, "Login") == 0)
		{
			printf("Login >>\n");
			client->SendData(&login);
		}
		else if (strcmp(cmdBuf, "Logout") == 0)
		{
			printf("Logout >>\n");
			client->SendData(&logout);
		}
		else {
			printf("???\n");
			continue;
		}
	}
}

int main() 
{
	EasyTcpClient client;
	//client.InitSocket();
	client.Connect("127.0.0.1", 14567);

	std::thread t(cmdThread, &client);
	t.detach();
	int num = 0;
		
	while (client.isRun())
	{
		client.OnRun();
	}
	client.Close();

	
	getchar();//for don"t close

	return 0;
}
