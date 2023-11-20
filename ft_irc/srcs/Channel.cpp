#include "../incs/irc.hpp"


// Adding and Removing Users
void Channel::addUser(int clientFd) {
	// Add client to the channel
	users.push_back(clientFd);
}
void Channel::addUser(const User& user) {
	// Add client to the channel
	userObjects.push_back(user);
}

void Channel::addUserToChannel(const User& user) {
	// Add client to the channel
	memberList.push_back(user);
}

void Channel::removeUser(int clientFd) {
	// remove client from the channel
	users.erase(std::remove(users.begin(), users.end(), clientFd), users.end());
}
void Channel::removeUser(const User& user) {
	// remove client from the channel
	userObjects.erase(std::remove(userObjects.begin(), userObjects.end(), user), userObjects.end());
}

void Channel::removeUserFromChannel(const User& user) {
	// remove client from the channel
	memberList.erase(std::remove(memberList.begin(), memberList.end(), user), memberList.end());
}

void Channel::broadcastMessage(const std::string &message) {
	// Send message to all users in the channel
				std::cout << "broadcast funct" << std::endl;
	for (std::vector<User>::size_type i = 0; i < userObjects.size(); i++) {
		send(userObjects[i].getFileDescriptor(), message.c_str(), message.length(), 0);
	}
}

// Kicking and inviting users
void Channel::kickUser(const User& operatorUser, const User& user) {
	// Check if operator
	if (!operatorUser.getIsoperator()) {
		send(operatorUser.getFileDescriptor(), "You are not an operator of this channel", 40, 0);
		return;
	}
	// Check if client is in the channel
	if (!isUserInChannel(user.getFileDescriptor())) {
		send(operatorUser.getFileDescriptor(), "This client is not in the channel", 40, 0);
		return;
	}
	// Kick the client
	send(user.getFileDescriptor(), "You have been kicked from the channel", 40, 0);
	removeUser(user);
}

void Channel::inviteUser(const User& operatorUser, const User& user) {
	// Check if operator
	if (!operatorUser.getIsoperator()) {
		send(operatorUser.getFileDescriptor(), "You are not an operator of this channel", 40, 0);
		return;
	}
	// Check if client is in the channel
	if (isUserInChannel(user.getFileDescriptor())) {
		send(operatorUser.getFileDescriptor(), "This client is already in the channel", 40, 0);
		return;
	}
	// Invite the client
	send(user.getFileDescriptor(), "You have been invited to the channel", 40, 0);
	addUser(user);
}

// Setting Topic and Mode
void Channel::setTopic(const User& user, const std::string &newTopic) {
	// Check if operator
	if (!user.getIsoperator()) {
		send(user.getFileDescriptor(), "You are not an operator of this channel", 40, 0);
		return;
	}
	// Set the topic
	channeltopic = newTopic;
}

// Private and  Helper Methods
void Channel::setInviteOnly (bool status) {
	this->inviteOnly = status;
}

void Channel::setTopicRestriction (const User& OperatorUser, bool restriction) {
	// Check if operator
	if (!OperatorUser.getIsoperator()) {
		send(OperatorUser.getFileDescriptor(), "You are not an operator of this channel", 40, 0);
		return;
	}
	// Set the topic restriction
	this->topicRestrictedToOps = restriction;
}

void Channel::setChannelPasword(const std::string& password) {
	this->channelPassword = password;
}

void Channel::setOperatorStatus(const std::string& nickname, bool status) {
	// Find the user
	for (std::vector<User>::size_type i = 0; i < userObjects.size(); i++) {
		if (userObjects[i].getNickname() == nickname) {
			userObjects[i].setIsOperator(status);
			return;
		}
	}
}



//  Set/Remove the User Limit to Channel
void Channel::setUserLimit(int limit) {
	this->userLimit = limit;
	// add logic to handle the case where the current number of users
	// exceeds the newly set limit.
	if (userObjects.size() > static_cast<std::vector<User>::size_type>(limit)) {
		// Remove users until the limit is reached
		while (userObjects.size() > static_cast<std::vector<User>::size_type>(limit)) {
			removeUser(userObjects[0]);
		}
	}

}



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

// void Channel::onNewConnection(int clientFd) {
// 	User newUser(clientFd);
// 	// Add the user to the list of users
// 	userObjects.push_back(newUser);
// }


// void Channel::onNewConnection(int clientFd) {
// 	// Send welcome message to the newly connected client
// 	// const char* WelcomeMessage = "Welcome to the IRC Server! Enter the Password\n ";
// 	// if(send(clientFd, WelcomeMessage, strlen(WelcomeMessage), 0) < 0) {
// 	// 	perror("Send failed in onNewConnection()");
// 	// }
// 	User newUser(clientFd);
// 	// Add the user to the list of users
// 	userObjects.push_back(newUser);
// }