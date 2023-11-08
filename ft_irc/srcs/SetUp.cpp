#include "../incs/irc.hpp"

int Socket::NickName(int i, std::string Mes){
	stats[i].NName = Mes;
	const char* WelcomeMessage = "NickName set\n";

	send(pfd[i].fd, WelcomeMessage, strlen(WelcomeMessage), 0);
	return 1;
}

int Socket::UserName(int i, std::string Mes){
	stats[i].UName = Mes;
	const char* WelcomeMessage = "User Name set\n";

	send(pfd[i].fd, WelcomeMessage, strlen(WelcomeMessage), 0);
	return 1;
};

bool	Socket::CmdCheck(int i, std::string Mes){
	if (Mes.compare(0, 5, "NName"))
		return NickName(i, Mes.substr(6, (Mes.npos - 5)));
	else if (Mes.compare(0, 5, "UName"))
		return UserName(i, Mes.substr(6, (Mes.npos - 5)));
	return 0;
}