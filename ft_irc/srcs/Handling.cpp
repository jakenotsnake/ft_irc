#include "../incs/irc.hpp"

int Socket::directMessage(int clientfd, const std::string& message){
    for (int c = 1; c < 10; c++){
		if (Mes.compare(0, stats[c].UName.length() , stats[c].UName) == 0){
			return DMessage(i, c, message);
		}
        if (c == 10){
            send(pfd[clientfd].fd, "User name does not exist", strlen("User name does not exist"), 0);
        }
	}
	return 0;
}

int	Socket::DMessage(int i, int c, const std::string& Mes){
	char *text = new char[stats[i].NName.length() + 1];
	strcpy(text, stats[i].NName.c_str());

	strcat(text, ":");
	for (int i = 1; i++; Mes[i]){
        strcat(text, " ");
        strcat(text, Mes[i]);
    }
	send(pfd[c].fd, text, strlen(text), 0);
	return 1;
}