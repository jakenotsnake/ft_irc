#include "../incs/irc.hpp"



void Channel::addUser(int clientFd) {
	// Add client to the channel
	users.push_back(clientFd);
}

void Channel::removeUser(int clientFd) {
	// remove client from the channel
	users.erase(std::remove(users.begin(), users.end(), clientFd), users.end());
}

void Channel::broadcastMessage(const std::string &message) {
	for (int userFd : users) {
		send(userFd, message.c_str(), message.length(), 0);
	}
}
