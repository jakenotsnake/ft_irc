#include "../incs/irc.hpp"

irc::irc(char **args){
    std::cout << "Starting server..." << std::endl;

    std::string Port = args[1];
    std::string Pass = args[2];
	UserM = new UserManager();
    Sock = new Socket(Port, Pass, UserM);
    // Sock->setServerPassword("1234");
};

irc::~irc(){
    std::cout << "Closing Server" << std::endl;
	delete Sock;
    delete UserM;
}
