#include "../incs/irc.hpp"

irc::irc(char **args){
	std::cout << "Starting server..." << std::endl;
	Sock = new Socket(args);
	// Sock->setServerPassword("1234");
};

irc::~irc(){
	std::cout << "Closing Server" << std::endl;
}
