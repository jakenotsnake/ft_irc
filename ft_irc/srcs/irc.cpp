#include "../incs/irc.hpp"

irc::irc(char **args){
	std::cout << "Starting server..." << std::endl;
	args = NULL;
	//this->Port = args[1];
	//this->Pass = args[2];
	std::cout << "1" << std::endl;
	Sock = new Socket();
};

irc::~irc(){
	std::cout << "Closing Server" << std::endl;
}
