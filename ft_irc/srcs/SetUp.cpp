#include "../incs/irc.hpp"

int Socket::NickName(int i, std::string Mes){
	Mes.erase(std::remove(Mes.begin(), Mes.end(), '\n'), Mes.cend());
	stats[i].NName = Mes;
	const char* WelcomeMessage = "NickName set\n";

	send(pfd[i].fd, WelcomeMessage, strlen(WelcomeMessage), 0);
	return 1;
}

int Socket::UserName(int i, std::string Mes){
	Mes.erase(std::remove(Mes.begin(), Mes.end(), '\n'), Mes.cend());
	stats[i].UName = Mes;
	const char* WelcomeMessage = "User Name set\n";

	send(pfd[i].fd, WelcomeMessage, strlen(WelcomeMessage), 0);
	return 1;
};

bool	Socket::CmdCheck(int i, std::string Mes){
	if (Mes.length() >= 5){
		if (Mes.compare(0, 5, "NName") == 0)
			return NickName(i, Mes.substr(6, (Mes.npos - 5)));
		else if (Mes.compare(0, 5, "UName") == 0)
			return UserName(i, Mes.substr(6, (Mes.npos - 5)));
		else if (Mes.compare(0, 7, "Message") == 0)
			return DirectMessage(i, Mes.substr(8, (Mes.npos - 8)));
	}
	return 0;
}

int	Socket::DirectMessage(int i, std::string Mes){
	for (int c = 1; c < 10; c++){
		if (Mes.compare(0, stats[c].UName.length() , stats[c].UName) == 0){
			return DMessage(i, c, Mes.substr(stats[c].UName.length() + 1, (Mes.npos - stats[c].UName.length())));
		}
	}
	return 0;
}

int	Socket::DMessage(int i, int c, std::string Mes){
	char *text = new char[stats[i].NName.length() + 1];
	strcpy(text, stats[i].NName.c_str());

	strcat(text, ": ");
	strcat(text, Mes.c_str());
	send(pfd[c].fd, text, strlen(text), 0);
	std::cout << "|" << Mes << std::endl;
	std::cout << stats[c].UName << "|" << c << std::endl;
	return 1;
}