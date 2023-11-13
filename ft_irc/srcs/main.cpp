#include <iostream>
#include <csignal>
#include "../incs/irc.hpp"

int main(int /*ac*/, char **args, char /*envp*/**){
	// ac = 2;
	// envp = NULL;
	irc maain = irc(args);
	return 1;
}