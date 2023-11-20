#include "../incs/irc.hpp"

int Socket::directMessage(int clientfd, std::vector<std::string> Mes){
    for (int c = 1; c < 10; c++){
		if (Mes[1] == stats[c].UName){
			return DMessage(clientfd, c, Mes);
		}
	}
	send(clientfd, "User name does not exist\n", strlen("User name does not exist\n"), 0);
	return 0;
}

int	Socket::DMessage(int i, int c, std::vector<std::string> Mes){
	char *text = new char[stats[i].NName.length() + 1];
	strcpy(text, stats[i].NName.c_str());

	strcat(text, ":");

	for (int i = 1; i < (int)Mes.size(); i++){
		strcat(text, " ");
        strcat(text, Mes[i].c_str());
	}
	strcat(text, "\n");
	send(c, text, strlen(text), 0);
	return 1;
}

int Socket::NickName(int i, std::string Mes){
	Mes.erase(std::remove(Mes.begin(), Mes.end(), '\n'), Mes.cend());
	stats[i].NName = Mes;
	if (stats[i].UName.empty())
		stats[i].UName = Mes;
	const char* WelcomeMessage = "NickName set\n";
	std::cout << "Client No." << i << " set NickName to " << Mes << std::endl;
	send(i, WelcomeMessage, strlen(WelcomeMessage), 0);
	return 1;
}

int Socket::UserName(int i, std::string Mes){
	Mes.erase(std::remove(Mes.begin(), Mes.end(), '\n'), Mes.cend());
	stats[i].UName = Mes;
	if (stats[i].NName.empty())
		stats[i].NName = Mes;
	const char* WelcomeMessage = "User Name set\n";
	std::cout << "Client No." << i << " set UserName to " << Mes << std::endl;
	send(i, WelcomeMessage, strlen(WelcomeMessage), 0);
	return 1;
}