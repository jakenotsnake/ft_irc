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
#include <sstream>

const int MAX_CLIENTS = 10;
extern const int BUFFER_SIZE;

class Channel {
public:
	std::string name;
	std::string topic;
	std::vector<int> users; // using client socket file descriptors as identifiers

	Channel() {} // Default constructor
	Channel(const std::string &name) : name(name) {}
	void addUser(int clientFd);
	void removeUser(int clientFd);
	void seTopic(const std::string &newTopic);
	void broadcastMessage(const std::string &message);

	

};


class	PfdStats{
public:
	bool	Pass;
	int		Chanel;
	int		Level;

};




class Socket
{
	std::string serverPassword;
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

	
	std::map<std::string, Channel> channels;

	Socket(const std::string &password);
	~Socket();
	int MakeSocket();
	int	OpenSocket();
//	int	ReadSocket(int i);
	int Polling();
	int Welcome(int i);
	int Pass(int i);
	// int Handle(int i, std::string Mes);


	int Handle(int i);
	void createChannel(int clientFd, const std::string &channelName);
	void joinChannel(int clientFd, const std::string &channelName);
	void leaveChannel(int clientFd, const std::string &channelName);
	void setServerPassword(const std::string& password) {std::cout<<"password set"<<std::endl;serverPassword = password;}
	bool validatePassword(int clientFd, const std::string& receivedPassword);
	void processClientCommand(int clientFd, const std::string& receivedData);
	void listChannels(int clientFd);
	void handleClientDisconnection(int clientIndex);
	void handleReadError(int clientInde);
	void sendClientMessage(int clientFd, const std::string& message);
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