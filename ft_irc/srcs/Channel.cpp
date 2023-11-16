#include "../incs/irc.hpp"



void Channel::addUser(int clientFd) {
	// Add client to the channel
	users.push_back(clientFd);
}

void Channel::removeUser(int clientFd) {
	// remove client from the channel
	users.erase(std::remove(users.begin(), users.end(), clientFd), users.end());
}

// void Channel::broadcastMessage(const std::string &message) {
// 	// Send message to all users in the channel
// 	for (int i = 0; i < users.size(); i++) {
// 		send(users[i].getFileDescriptor(), message.c_str(), message.length(), 0);
// 	}
// }

bool Channel::isUserInChannel(int clientFd) {
	// Check if client is in the channel
	if (std::find(users.begin(), users.end(), clientFd) != users.end()) {
		return true;
	}
	return false;
}

bool Channel::isOperator(int clientFd) {
	// Check if client is in the channel
	if (std::find(users.begin(), users.end(), clientFd) != users.end()) {
		return true;
	}
	return false;
}

// void Channel::kickUser(int operatorFd, int clientFd) {
// 	// Check if operator
// 	if (!isOperator(operatorFd)) {
// 		send(operatorFd, "You are not an operator of this channel", 40, 0);
// 		return;
// 	}
// 	// Check if client is in the channel
// 	if (!isUserInChannel(clientFd)) {
// 		send(operatorFd, "This client is not in the channel", 40, 0);
// 		return;
// 	}
// 	// Kick the client
// 	send(clientFd, "You have been kicked from the channel", 40, 0);
// 	removeUser(clientFd);
// }


// void Channel::inviteUser(int operatorFd, int clientFd) {
// 	// Check if operator
// 	if (!isOperator(operatorFd)) {
// 		send(operatorFd, "You are not an operator of this channel", 40, 0);
// 		return;
// 	}
// 	// Check if client is in the channel
// 	if (isUserInChannel(clientFd)) {
// 		send(operatorFd, "This client is already in the channel", 40, 0);
// 		return;
// 	}
// 	// Invite the client
// 	send(clientFd, "You have been invited to the channel", 40, 0);
// 	addUser(clientFd);
// }

// void Channel::setTopic(int operatorFd, const std::string &newTopic) {
// 	// Check if operator
// 	if (!isOperator(operatorFd)) {
// 		send(operatorFd, "You are not an operator of this channel", 40, 0);
// 		return;
// 	}
// 	// Set the topic
// 	topic = newTopic;
// }

// void Channel::setMode((int operatorFd, char mode, int clientFd) {
// 	if (!isOperator) {
// 		send(operatorFd, "You are not an operator of this channel", 40, 0);
// 		return;
// 	}
// 	switch (mode) {
// 		case 'i' : setIn
// 	}
// }