#include "../incs/irc.hpp"

int	Socket::Welcome(int i){
	const char* WelcomeMessage = "Welcome to Joseph and Jakes IRC Server!\n ";

	int BytesSent = send(pfd[i].fd, WelcomeMessage, strlen(WelcomeMessage), 0);
	if (BytesSent < 0) {
        std::cerr << "Failed to send welcome message to the client." << std::endl;
    } else {
        std::cout << "Welcome message sent to the client." << std::endl;
    }
	return 1;
}

int Socket::Pass(int i){
	const char* Message = "Enter Password: ";
	stats[i].Chanel = 0;
	stats[i].Level = 0;
	stats[i].Pass = 0;

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
		PassCheck(i, Mes);
	else 
		std::cout << Mes << std::endl;
	return 1;
}

int Socket::PassCheck(int i, std::string Mes){
	const char* Message = "Wrong Pass, Try again\n";

	if (Mes.compare(PassW) == 1){
		stats[i].Pass = 1;
		send(pfd[i].fd, "Correct!\n", strlen("Correct!\n"), 0);
		Welcome(i);
	}
	else {
		send(pfd[i].fd, Message, strlen(Message), 0);
		stats[i].Pass = 0;
	}
	std::cout << "entered: " << Mes << std::endl;
	std::cout << "Actual: : " << PassW << std::endl;
	return 0;
}