#include "../incs/irc.hpp"


// Adding and Removing Users
void Channel::addUser(const std::string& nickname, int clientFd) {
	std::cout << "Username :" << nickname << std::endl;
	// Add client to the channel
	users[nickname] = clientFd;
}


void Channel::removeUser(const std::string& nickname) {
	// remove client from the channel
	users.erase(nickname);
}

void Channel::listUsers(int clientFd) {
	// debug: print inside of the function
	std::cout << "Inside listUsers()" << std::endl;

	// Send the list of users to the client
	std::string userList;
	for (std::map<std::string, int>::iterator it = users.begin(); it != users.end(); ++it) {
		userList += it->first + "\n";
	}
	send(clientFd, userList.c_str(), userList.length(), 0);
}

void Channel::setChannelOperator(int clientFd) {
	// debug: print inside of the function
	std::cout << "Inside setChannelOperator()" << std::endl;

	// Set the channel operator	
	channelOperator = clientFd;
	
}


bool Channel::isOperator(int clientFd) {
	// debug: print inside of the function
	std::cout << "Inside isOperator()" << std::endl;

	// Check if client is the channel operator
	if (clientFd == channelOperator) {
		return true;
	}
	return false;
}

int Channel::getFileDescriptor(const std::string& nickname) {
	// debug: print inside of the function
	std::cout << "Inside getFileDescriptor()" << std::endl;

	// Find the user
	if (userManager) {
		PfdStats* pfdStats = userManager->getPfdStats(nickname);
		if (pfdStats) {
			return pfdStats->getFileDescriptor();
		}
	}

	return -1;
}

bool Channel::isUserInChannel(const std::string& nickname) {
	// debug: print inside of the function
	std::cout << "Inside isUserInChannel()" << std::endl;

	// Check if client is in the channel
	if (users.find(nickname) != users.end()) {
		return true;
	}
	return false;

}

void Channel::kickUser(int clientFd, std::string& nickname) {
	// debug: print inside of the function
	std::cout << "Inside kickUser()" << std::endl;

	// Check if operator
	if (!isOperator(clientFd)) {
		send(clientFd, "You are not an operator of this channel", 40, 0);
		return;
	}
	// Check if client is in the channel
	if (!isUserInChannel(nickname)) {
		send(clientFd, "This client is not in the channel", 40, 0);
		return;
	}
	// Kick the client
	send(getFileDescriptor(nickname), "You have been kicked from the channel", 40, 0);
	removeUser(nickname);
}

void Channel::inviteUser(int clientFd, std::string& nickname) {
	// debug: print inside of the function
	std::cout << "Inside inviteUser()" << std::endl;

	// Check if operator
	if (!isOperator(clientFd)) {
		send(clientFd, "You are not an operator of this channel", 40, 0);
		return;
	}
	// Check if client is in the channel
	if (isUserInChannel(nickname)) {
		send(clientFd, "This client is already in the channel", 40, 0);
		return;
	}
	int targetFd = getFileDescriptor(nickname);
	if (targetFd != -1) {
		// invite the client
		std::string inviteMessage = "You have been invited to the channel " + channelName;
		send(targetFd, inviteMessage.c_str(), inviteMessage.length(), 0);
	} else {
		// notify the operator that the client is not online
		send(clientFd, "This client is not online", 40, 0);
	}
}

void Channel::setTopic(int clientFd, const std::string& newTopic) {
	// debug: print inside of the function
	std::cout << "Inside setTopic()" << std::endl;

	// Check if operator
	if (!isOperator(clientFd)) {
		send(clientFd, "You are not an operator of this channel", 40, 0);
		return;
	}
	// Set the topic
	channeltopic = newTopic;
}

void Channel::setInviteOnly (bool status) {
	// debug: print inside of the function
	std::cout << "Inside setInviteOnly()" << std::endl;

	this->inviteOnly = status;
}

void Channel::setTopicRestriction (int clientFd, bool restriction) {
	// debug: print inside of the function
	std::cout << "Inside setTopicRestriction()" << std::endl;

	// Check if operator
	if (!isOperator(clientFd)) {
		send(clientFd, "You are not an operator of this channel", 40, 0);
		return;
	}
	// Set the topic restriction
	this->topicRestrictedToOps = restriction;
}

void Channel::setChannelPasword(const std::string& password) {
	// debug: print inside of the function
	std::cout << "Inside setChannelPasword()" << std::endl;

	this->channelPassword = password;
}

void Channel::setUserLimit(int limit) {
	// debug: print inside of the function
	std::cout << "Inside setUserLimit()" << std::endl;

	this->userLimit = limit;
	// add logic to handle the case where the current number of users
	// exceeds the newly set limit.
	if (users.size() > static_cast<std::map<std::string, int>::size_type>(limit)) {
		// Remove users until the limit is reached
		while (users.size() > static_cast<std::map<std::string, int>::size_type>(limit)) {
			removeUser(users.begin()->first);
		}
	}
}

void Channel::setMode(int clientFd, const std::string& mode, const std::string& modeParameter) {
	if (!isOperator(clientFd)) {
		send(clientFd, "You are not an operator of this channel", 40, 0);
		return;
	}
	if (mode == "t") {
		setTopicRestriction(clientFd, modeParameter == "on" ? true : false);
	}
	else if (mode == "i") {
		setInviteOnly(modeParameter == "on" ? true : false);
	}
	else if (mode == "l") {
		setUserLimit(std::stoi(modeParameter));
	}
	else if (mode == "k") {
		setChannelPasword(modeParameter);
	}
	else if (mode == "o") {
		setChannelOperator(getFileDescriptor(modeParameter));
	}
	else {
		send(clientFd, "Invalid mode", 40, 0);
	}
}



// Sending Messages
void Channel::broadcastMessage(const std::string &senderNickname, const std::string &message) {
	// debug: print inside of the function
	std::cout << "Inside broadcastMessage()" << std::endl;

	std::string broadcastMessage = senderNickname + ": " + message;
	for (std::map<std::string, int>::iterator it = users.begin(); it != users.end(); ++it) {
		send(it->second, broadcastMessage.c_str(), broadcastMessage.length(), 0);
	}
}

