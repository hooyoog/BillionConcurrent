#ifndef _MESSAGE_HEAD_CPP_
#define _MESSAGE_HEAD_CPP_

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
#endif // !_MESSAGE_HEAD_CPP_

