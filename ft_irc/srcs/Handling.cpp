#include "../incs/irc.hpp"

int	Socket::Welcome(int i){
	const char* WelcomeMessage = "Welcome to the IRC Server!\n";

	stats[i].Chanel = 0;
	stats[i].Level = 0;
	stats[i].Pass = 0;
	int BytesSent = send(pfd[i].fd, WelcomeMessage, strlen(WelcomeMessage), 0);
	if (BytesSent < 0) {
        std::cerr << "Failed to send welcome message to the client." << std::endl;
    } else {
        std::cout << "Welcome message sent to the client." << std::endl;
    }
	Pass(i);
	return 1;
}

int Socket::Pass(int i){
	const char* Message = "Enter Password: ";

	int BytesSent = send(pfd[i].fd, Message, strlen(Message), 0);
	if (BytesSent < 0) {
        std::cerr << "Failed to send welcome message to the client." << std::endl;
    } else {
        std::cout << "Welcome message sent to the client." << std::endl;
    }
	return 1;
}

int Socket::Handler(int i, std::string Mes){
	if (stats[i].Pass == 0)
		stats[i].Pass = PassCheck(i, Mes);
}