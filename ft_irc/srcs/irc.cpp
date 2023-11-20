#include "../incs/irc.hpp"

irc::irc(char **args){
	std::cout << "Starting server..." << std::endl;
	//this->Port = args[1];
	//this->Pass = args[2];
	std::cout << args[0] << std::endl;
	Sock = new Socket(args);
};

irc::~irc(){
	std::cout << "Closing Server" << std::endl;
}
