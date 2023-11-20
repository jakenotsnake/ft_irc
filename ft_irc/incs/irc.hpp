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
	// constructor with file descriptor and nickname
	User(int fd, const std::string& nickname) : fileDescriptor(fd), nickname(nickname), isOperator(false) {}
	

	// constructor that takes file descriptor and sets a default nickname
	explicit User(int fd) : fileDescriptor(fd), isOperator(false) {
		std::ostringstream ss;
		ss << "user" << fd;
		nickname = ss.str();
	}

	// Getters and setters
	int getFileDescriptor() const {return fileDescriptor;}
	const std::string& getNickname() const {return nickname;}
	bool getIsoperator() const {return isOperator;}
	void setNickname(const std::string& newNickname) {nickname = newNickname;}
	void setIsOperator(bool status) {isOperator = status;}
	void onNewConnection(int fileDescriptor);

	bool operator==(const User& other) const {
		return fileDescriptor == other.fileDescriptor;
	}

private:
	int fileDescriptor;
	std::string nickname;
	bool isOperator;
	std::vector<User> userList;
};


class Channel {
	friend class Socket;
public:
	Channel() {} // Default constructor
	Channel(const std::string& name) : channelName(name), inviteOnly(false),
		topicRestrictedToOps(false), userLimit(-1) {}
	
	void addUser(int clientFd);
	void addUser(const User& user);
	void addUserToChannel(int clientFd);
	void addUserToChannel(const User& user);

	void removeUser(int clientFd);
	void removeUser(const User& user);
	void broadcastMessage(const std::string &message);
	void removeUserFromChannel(int clientFd);
	void removeUserFromChannel(const User& user);

	void setTopic(const User& user, const std::string &newTopic);
	void kickUser(const User& operatorUser, const User& user);
	void inviteUser(const User& operatorUser, const User& user);
	void setMode(const User& operatorUser, const std::string& mode);
	bool isUserInChannel(int clientFd);
	bool isOperator(int clientFd);

	
	
	


private:
	std::string channelName;
	std::string channeltopic;
	std::vector<User> userObjects;
	std::vector<User> memberList;
	std::vector<int> users;
	bool inviteOnly;
	bool topicRestrictedToOps;
	int userLimit;
	std::string channelPassword;

	void setInviteOnly(bool status);
	void setTopicRestriction(const User& OperatorUser, bool restriction);
	void setChannelPasword(const std::string& password);
	void setOperatorStatus(const std::string& nickname, bool status);
	// void setOperatorStatus(const User& user, bool status);
	void setUserLimit(int limit);
	


};

class ServerManagement {
public:
	ServerManagement() {}
	// void setServerPassword(const std::string& password) {serverPassword = password;}
	bool validatePassword(int clientFd, const std::string& receivedPassword);
	// void processClientCommand(int clientFd, const std::string& receivedData);
	// void listChannels(int clientFd);
	// void handleClientDisconnection(int clientIndex);
	// void handleReadError(int clientIndex);
	// void sendClientMessage(int clientFd, const std::string& message);
	// std::string getNickNameFromClientFd(int clientFd);
	void addUserToIrc(int clientFd);
	void removeUserFromIrc(int clientFd);

private: 
	std::vector<User> UserList;
	std::map<std::string, Channel> channels;
};


class	PfdStats{
public:
	bool	Pass;
	int		Chanel;
	int		Level;

};




class Socket
{
	friend class User;
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
	std::string getNickNameFromClientFd(int clientFd);
	int directMessage(int clientFd, const std::string& receivedData);
	int	DMessage(int i, int c, std::string& Mes);
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