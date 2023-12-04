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



class	PfdStats{
public:
	bool	Pass;
	int		Chanel;
	int		Level;
	std::string nickname;
	int fileDescriptor;
	// bool isOperator;

	int getFileDescriptor() const {
		//print the file descriptor of the user 
		std::cout << "Inside PfdStat's getFileDescriptor " << fileDescriptor << std::endl;
		return fileDescriptor;}
	const std::string& getNickname() const {return nickname;}
	void setNickname(const std::string& newNickname) {nickname = newNickname;}


};

class UserManager {
	std::map<std::string, PfdStats> OnlineUsers; // map of nicknames to pfdstats

public:
	UserManager(); // Default constructor
	~UserManager();// Default destructor
	void addUser(const std::string& nickname, const PfdStats& pfdStats);
	void removeUser(const std::string& nickname);
	PfdStats* getPfdStats(const std::string& nickname);
};



class Channel {
	UserManager* userManager; // pointer to the userManager
public:
	Channel() {} // Default constructor
	Channel(const std::string& name, UserManager* um) : userManager(um), channelName(name),  inviteOnly(false),
		topicRestrictedToOps(false), userLimit(-1), channelOperator(-1) {}
	
	void addUser(const std::string& nickname, int clientFd);
	void removeUser(const std::string& nickname);
	void broadcastMessage(const std::string &senderNickname, const std::string &message);
	bool isUserInChannel(const std::string& nickname);
	bool isOperator(int clientFd);
	void listUsers(int clientFd);
	void setChannelOperator(int clientFd);
	int getFileDescriptor(const std::string& nickname);

	// Channel operator functions
	void kickUser(int clientFd, std::string& nickname);
	void inviteUser(int clientFd, std::string& nickname);

	// Mode functions
	void setTopic(int clientFd, const std::string& newTopic);
	void setInviteOnly(bool status);
	void setTopicRestriction(int clientFd, bool restriction);
	void setChannelPasword(const std::string& password);
	void setUserLimit(int limit);
	void setMode(int clientFd, const std::string& mode, const std::string& modeParameter);

	// 
	
	


private:
	std::string channelName;
	std::string channeltopic;
	// std::vector<User> memberList;
	std::map<std::string, int> users;
	bool inviteOnly;
	bool topicRestrictedToOps;
	int userLimit;
	int channelOperator;
	std::string channelPassword;
	

};






class Socket
{
	std::string serverPassword;
	UserManager* userManager; // pointer to the userManager
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

	Socket(const std::string &password, UserManager* um);
	~Socket();
	int MakeSocket();
	int	OpenSocket();
	int Polling();
	int Welcome(int i);
	int Pass(int i);
	



	int Handle(int i);
	void createChannel(int clientFd, const std::string &channelName);
	void joinChannel(int clientFd, const std::string &channelName);
	void leaveChannel(int clientFd, const std::string &channelName);
	Channel* getChannel(int clientFd);

	void setServerPassword(const std::string& password) {std::cout<<"password set"<<std::endl;serverPassword = password;}
	bool validatePassword(int i, int clientFd, const std::string& receivedPassword);
	void processClientCommand(int clientFd, const std::string& receivedData, int clientIndex);
	bool isRecognizedCommand(const std::string& input);
	void listChannels(int clientFd);
	void listchannelUsers(int clientFd, const std::string& channelName);
	void handleClientDisconnection(int clientIndex);
	void handleReadError(int clientInde);
	void sendClientMessage(int clientFd, const std::string& message);
	std::string getNickNameFromClientFd(int clientFd);
	void setNickName(int clientFd, const std::string& nickname, int clientIndex);
	
};


class irc
{
	UserManager	*UserM; // pointer to the userManager
public:
	std::string	Port;
	std::string	Pass;
	Socket	*Sock;
	

	irc(char **args);
	~irc();
};







#endif