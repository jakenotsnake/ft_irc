#ifndef IRC_HPP
# define IRC_HPP

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <string>
#include <exception>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/time.h>
#include <arpa/inet.h>

const int MAX_CLIENTS = 10;

class Socket
{
public:
	sockaddr_in SocketAdrs, clientAddr;
	std::map<int, std::string> partialData;
	pollfd		pfd[20];
	std::vector<int>	ClientSockets;
	PfdStats	stats[20];
	int AdrsLen;
	int Socket_ID; 
	int AcptSocket;
	int ON;
	int PollRet;
	int	Flag;

	Socket(/* args */);
	~Socket();
	int MakeSocket();
	int	OpenSocket();
//	int	ReadSocket(int i);
	int Polling();
	int Welcome(int i);
	int Pass(int i);
	int Handle(int i, std::string Mes);
};

class irc
{
public:
	std::string	Port;
	std::string	Pass;
	Socket	*Sock;

	irc(char **args);
	~irc();
};

class	PfdStats{
public:
	bool	Pass;
	int		Chanel;
	int		Level;

};

#endif