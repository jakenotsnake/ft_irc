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

class User {
public:
	User(int fd, const std::string& nickname) : fileDescriptor(fd), nickname(nickname), isOperator(false) {}

	// Getters and setters
	int getFileDescriptor() const {return fileDescriptor;}
	const std::string& getNickname() const {return nickname;}
	bool getIsoperator() const {return isOperator;}
	void setNickname(const std::string& newNickname) {nickname = newNickname;}
	void setIsOperator(bool status) {isOperator = status;}

private:
	int fileDescriptor;
	std::string nickname;
	bool isOperator;
};


class Channel {
public:
	Channel() {} // Default constructor
	Channel(const std::string& name) : channelName(name), inviteOnly(false),
		topicRestrictedToOps(false), userLimit(-1) {}
	
	void addUser(int clientFd);
	void removeUser(int clientFd);
	void setTopic(const User& user, const std::string &newTopic);
	void broadcastMessage(const std::string &message);

	void kickUser(const User& operatorUser, const User& user);
	void inviteUser(const User& operatorUser, const User& user);
	void setMode(const User& operatorUser, const std::string& mode);
	bool isUserInChannel(int clientFd);
	bool isOperator(int clientFd);


private:
	std::string channelName;
	std::string channeltopic;
	// std::vector<User> users;
	std::vector<int> users;
	bool inviteOnly;
	bool topicRestrictedToOps;
	int userLimit;
	std::string channelPassword;

	void setInviteOnly(bool /*status*/) {}
	void setTopicRestriction(const User& OperatorUser, bool restriction);
	void setChannelPasword(const std::string& password);
	void setOperatorStatus(const std::string& nickname, bool status);
	void setUserLimit(int limit);


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
	bool validatePassword(int i, int clientFd, const std::string& receivedPassword);
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