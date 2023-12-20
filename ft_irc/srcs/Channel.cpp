#include "../incs/irc.hpp"


// Adding and Removing Users
bool Channel::addUser(const std::string& nickname, int clientFd) {
	std::cout << "Username :" << nickname << std::endl;
	// Add client to the channel
	int i = std::distance(users.begin(), users.end());
	if (i >= userLimit){
		send(clientFd, "Error: Too many users in chanel", strlen("Error: Too many users in chanel"), 0);
		return 0;
	}
	users[nickname] = clientFd;
	char *mes = new char[1000];
	strcat(mes, "Welcome to \"");
	strcat(mes, channelName.c_str());
	strcat(mes, "\" the topic of this channel is: ");
	strcat(mes, channeltopic.c_str());
	strcat(mes, "\n");
	send(clientFd, mes, strlen(mes), 0);

	return 1;
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

	// Add client to channel operators
	channelOperators.push_back(clientFd);
	
}

int Channel::getChannelOperatorCount() {
		// return count of items in the vector that contain a file descriptor
		int count = 0;
		for (std::vector<int>::size_type i = 0; i < channelOperators.size(); i++) {
			if (channelOperators[i] != 0) {
				count++;
			}
		}
		return count;
	}

void Channel::setNextUserNotOperatorAsOperator() {
	// debug: print inside of the function
	std::cout << "Inside setNextUserNotOperatorAsOperator()" << std::endl;

	// Set the next user in the channel as an operator
	for (std::map<std::string, int>::iterator it = users.begin(); it != users.end(); ++it) {
		if (!isOperator(it->second)) {
			setChannelOperator(it->second);
			// debug: print the nickname of the user that was set as an operator
			std::cout << "set " << it->first << " as an operator" << std::endl;
			return;
		}
	}
}


bool Channel::isOperator(int clientFd) {
	// debug: print inside of the function
	std::cout << "Inside isOperator()" << std::endl;

	// Check if client is in channel operators
	for (std::vector<int>::iterator it = channelOperators.begin(); it != channelOperators.end(); ++it) {
		if (*it == clientFd) {
			return true;
		}
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

		// Add user to invite list
		inviteList.push_back(targetFd);
	} else {
		// notify the operator that the client is not online
		send(clientFd, "This client is not online", 40, 0);
	}
}

void Channel::removeChannelOperator(int clientFd) {
	// debug: print inside of the function
	std::cout << "Inside removeChannelOperator()" << std::endl;

	// Remove client from channel operators
	for (std::vector<int>::iterator it = channelOperators.begin(); it != channelOperators.end(); ++it) {
		if (*it == clientFd) {
			channelOperators.erase(it);
			return;
		}
	}
}

void Channel::removeUserFromInviteList(int clientFd) {
	// debug: print inside of the function
	std::cout << "Inside removeUserFromInviteList()" << std::endl;

	// Remove client from invite list
	for (std::vector<int>::iterator it = inviteList.begin(); it != inviteList.end(); ++it) {
		if (*it == clientFd) {
			inviteList.erase(it);
			return;
		}
	}
}

bool Channel::isUserInvited(int clientFd) {
	// debug: print inside of the function
	std::cout << "Inside isUserInvited()" << std::endl;

	// Check if client is in invite list
	for (std::vector<int>::iterator it = inviteList.begin(); it != inviteList.end(); ++it) {
		if (*it == clientFd) {
			return true;
		}
	}
	return false;
}


void Channel::setTopic(int clientFd, const std::string& newTopic) {
	// debug: print inside of the function
	std::cout << "Inside setTopic()" << std::endl;

	// Check if channel is topic restricted
	if (topicRestrictedToOps) {
		// Check if operator
		if (!isOperator(clientFd)) {
			send(clientFd, "You are not an operator of this channel", 40, 0);
			return;
		} else {
			// Set the topic
			channeltopic = newTopic;
		}
	} else {
		// Set the topic
		channeltopic = newTopic;
	}

}


void Channel::setInviteOnly (bool status) {
	// debug: print inside of the function
	std::cout << "Inside setInviteOnly()" << std::endl;

	this->inviteOnly = status;

	// debug: print the status of inviteOnly
	std::cout << "inviteOnly: " << inviteOnly << std::endl;
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

	// debug: print the status of topicRestrictedToOps
	std::cout << "topicRestrictedToOps: " << topicRestrictedToOps << std::endl;
}

void Channel::setPasswordProtected(bool status) {
	// debug: print inside of the function
	std::cout << "Inside setPasswordProtected()" << std::endl;

	this->passwordProtected = status;

	// debug: print the status of passwordProtected
	std::cout << "passwordProtected: " << passwordProtected << std::endl;
}

void Channel::setChannelPasword(const std::string& password) {
	// debug: print inside of the function
	std::cout << "Inside setChannelPasword()" << std::endl;

	this->channelPassword = password;
	// debug: print the status of channelPassword
	std::cout << "channelPassword: " << channelPassword << std::endl;
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
	// send message to all channel operators that the limit has been set
	std::string message = "The user limit has been set to " + std::to_string(limit);
	for (std::vector<int>::iterator it = channelOperators.begin(); it != channelOperators.end(); ++it) {
		send(*it, message.c_str(), message.length(), 0);
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
		setPasswordProtected(modeParameter == "on" ? true : false);
	}
	else if (mode == "o") {
		setChannelOperator(clientFd);
	}
	else if (mode == "l") {
		setUserLimit(std::stoi(modeParameter));
	}
	else {
		send(clientFd, "Invalid mode", 40, 0);
	}
}



// Sending Messages
void Channel::broadcastMessage(const std::string &senderNickname, const std::string &message) {
	// debug: print inside of the function
	std::cout << "Inside broadcastMessage()" << std::endl;

	std::string broadcastMessage = senderNickname + ": " + message + "\n";
	for (std::map<std::string, int>::iterator it = users.begin(); it != users.end(); ++it) {
		send(it->second, broadcastMessage.c_str(), broadcastMessage.length(), 0);
	}
}

