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

class	PfdStats{
public:
	bool	Pass;
	int		Chanel;
	int		Level;
	std::string	UName;
	std::string	NName;
	bool step;
};

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
	std::string PassW;

	Socket(/* args */);
	~Socket();
	int MakeSocket();
	int	OpenSocket();
//	int	ReadSocket(int i);
	int Polling();
	int Welcome(int i);
	int Pass(int i);
	int Handler(int i, std::string Mes);
	int	PassCheck(int i, std::string Mes);
	int NickName(int i, std::string Mes);
	int UserName(int i, std::string Mes);
	int	setup(int i, std::string Mes);
	bool	CmdCheck(int i, std::string Mes);
	int		DirectMessage(int i, std::string Mes);
	int		DMessage(int i, int c, std::string Mes);
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



#endif