#include "../incs/irc.hpp"

void User::onNewConnection(int clientFd) {
	User newUser(clientFd);
	// Add the user to the list of users
	userList.push_back(newUser);
}
