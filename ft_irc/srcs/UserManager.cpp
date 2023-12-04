#include "../incs/irc.hpp"

UserManager::UserManager() {
    // debug: print inside of the function
    std::cout << "UserManager Constructed" << std::endl;
}

UserManager::~UserManager() {
    // debug: print inside of the function
    std::cout << "UserManager Destructed" << std::endl;
}

void UserManager::addUser(const std::string& nickname, const PfdStats& pfdStats) {
    // debug: print inside of the function
    std::cout << "Inside addUser()" << std::endl;

    // check if nickname is a valid string
    if (nickname.empty()) {
        std::cout << "Invalid nickname" << std::endl;
        return;
    }
	// print name and file descriptor of the user added
	std::cout << "nickname: " << nickname << std::endl;
	std::cout << "file descriptor: " << pfdStats.getFileDescriptor() << std::endl;

    // Add the user to the list of users
    OnlineUsers[nickname] = pfdStats;
}

void UserManager::removeUser(const std::string& nickname) {
    // debug: print inside of the function
    std::cout << "Inside removeUser()" << std::endl;

    // Remove the user from the list of users
    OnlineUsers.erase(nickname);
}

PfdStats* UserManager::getPfdStats(const std::string& nickname) {
    // debug: print inside of the function
    std::cout << "Inside getPfdStats()" << std::endl;

    // Find the user
    for (std::map<std::string, PfdStats>::iterator it = OnlineUsers.begin(); it != OnlineUsers.end(); ++it) {
		// debug: print every nickname in the map		
		std::cout << it->first << std::endl;
        if (it->first == nickname) {
			// print the file descriptor of the user
			std::cout << it->second.getFileDescriptor() << std::endl;
            return &it->second;
        }
    }
    return nullptr;
}
