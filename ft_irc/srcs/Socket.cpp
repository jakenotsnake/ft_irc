#include "../incs/irc.hpp"

const int BUFFER_SIZE = 999;

//Constructor set easy variable;
Socket::Socket(const std::string &password) : serverPassword(password) {
	//struct to hold port/adresses
	SocketAdrs = sockaddr_in();
	
	//adress family (IPv4)
	SocketAdrs.sin_family = AF_INET;
	//port
	SocketAdrs.sin_port = htons(6667);
	//allowing info to be received from any network
	SocketAdrs.sin_addr.s_addr = INADDR_ANY;
	//getting size of SocketAdrs variable
	AdrsLen = sizeof(SocketAdrs);
	
	
	
	MakeSocket();
}

//Make and set the Socket
int Socket::MakeSocket()
{
	ON = 1;
	//socket creation AF_INET(family, IPv4), Sock_stream(com type, TCP), 0(protocol, internet)
	if ((Socket_ID = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Socket failed");
		exit(EXIT_FAILURE);
	}
	//setting adress to be reusable
	if ((setsockopt(Socket_ID, SOL_SOCKET, SO_REUSEADDR, &ON, sizeof(ON))) < 0){
		perror("Set Opt failed");
		exit(EXIT_FAILURE);
	}
	//retrieve socket flag
	if ((Flag = fcntl(Socket_ID, F_GETFL, 0)) < 0){
		perror("Flag failed");
		exit(EXIT_FAILURE);
	}
	//set flag to non blocking
	Flag |= O_NONBLOCK;
	if ((fcntl(Socket_ID, F_SETFL, Flag)) < 0){
		perror("Set Opt failed");
		exit(EXIT_FAILURE);
	}
	//bind socket to info set in constructor
	if ((bind(Socket_ID, ((const sockaddr*) &SocketAdrs), AdrsLen)) < 0){
		perror("Bind failed");
		exit(EXIT_FAILURE);
	}
	//set poll file descriptors to -1
	 for (int i = 1; i <= MAX_CLIENTS; ++i) {
		pfd[i].fd = -1;
	}
	if ((OpenSocket()) == -1)
		return (-1);
	else
		return (1);
}

//Open the socket to listen to other sockets.
int Socket::OpenSocket(){
	//printing info
	char ipStr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(SocketAdrs.sin_addr), ipStr, INET_ADDRSTRLEN);

	std::cout << "Local Address: " << ipStr << std::endl;
	std::cout << "Local Port: " << ntohs(this->SocketAdrs.sin_port) << std::endl;
	//set Socket_ID to accept incoming connect, number is max connect
	if ((listen(Socket_ID, 3)) < 0)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}
	pfd[0].fd = Socket_ID;
	pfd[0].events = POLLIN;
	Polling();
	return (1);
}

int Socket::Polling(){
	while(true){
		//wait for event
		PollRet = poll(pfd, 10, (60 * 10000));
		if (PollRet < 0){
			perror("Poll() failed");
	   		exit(EXIT_FAILURE);
			}
			//checking if event was on listening socket (means new connection)
		if (pfd[0].revents & POLLIN){
			//accept incoming connection
			if ((AcptSocket = accept(pfd[0].fd,(struct sockaddr *)&clientAddr, (socklen_t *)&AdrsLen)) < 0){
				std::cerr << "Failed to accept connection" << std::endl;
				return 1;
				}
			//loging IP adress of client
			std::string clientAddress = inet_ntoa(clientAddr.sin_addr);
			std::cout << "New client connected: " << clientAddress << std::endl;
			//finding available pfd socket
			for (int i = 1; i <= MAX_CLIENTS; ++i) {
					if (pfd[i].fd == -1) {
						pfd[i].fd = AcptSocket;
						pfd[i].events = POLLIN;
						ClientSockets.push_back(AcptSocket);
						Welcome(i);
						break;
					}
				}
			}
		for (int i = 1; i <= MAX_CLIENTS; ++i) {
			if (pfd[i].fd != -1 && (pfd[i].revents & POLLIN)) {
				//receive data and hold in buff
				char buff[BUFFER_SIZE];
				int bytesRead = recv(pfd[i].fd, buff, BUFFER_SIZE, 0);
				//convert buff to string
				std::string buffer(buff);
				buffer.erase(bytesRead, std::string::npos);
				//check if client connect is closed
				if (bytesRead <= 0) {
				close(pfd[i].fd);
				pfd[i].fd = -1;
				ClientSockets.erase(std::remove(ClientSockets.begin(), ClientSockets.end(), pfd[i].fd), ClientSockets.end());
				}
				//read message
				else {
				std::cout << buffer << std::endl;
				}
			}	
		}
	}
	return 1;
}





///


void Socket::createChannel(int clientFd, const std::string &channelName) {
	//check if channel exists
	if (channels.find(channelName) != channels.end()) {
		sendClientMessage(clientFd, "Channel " + channelName + " already exists.\n");
		return;
	}

	// create a new channel
	channels.insert(std::make_pair(channelName, Channel(channelName)));
	// sendClientMessage(clientFd, "Channel " + channelName + " created.\n Type 'join' " + channelName + " to join or type 'commands' to see available commands.\n");
	// sendClientMessage(clientFd, "Channel created.\n");
	std::cout << "Channel " << channelName << " created." << std::endl;
	const std::string successMessage = "Channel " + channelName + " created.\n";
	send(clientFd, successMessage.c_str(), successMessage.length(), 0);
}

void Socket::joinChannel(int clientFd, const std::string &channelName) {
	// check if the channel exists
	std::map<std::string, Channel>::iterator it = channels.find(channelName);
	if(it == channels.end()) {
		std::cerr << "channelName" << " does not exist." << std::endl;
		//send an error message to the client
		const std::string errMsg = "channelName" + channelName + " does not exist.\n";
		send(clientFd, errMsg.c_str(), errMsg.length(), 0);
		return;
	}

	// Add the client to the channel's member list
	it->second.addUser(clientFd);
	std::cout << " client " << clientFd << " joined channel " << channelName << std::endl;
}

void Socket::leaveChannel(int clientFd, const std::string &channelName) {
	// check if channel exists
	std::map<std::string, Channel>::iterator it = channels.find(channelName);
	if (it == channels.end()) {
		std::cerr << "channel" << channelName << " does not exist" << std::endl ;
		return;
	}

	it->second.removeUser(clientFd);
	std::cout<< " Client " << clientFd << " left channel " << channelName << std::endl;
	}



// HELPER FUNCTION
std::string trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t\n\r\f\v");
	size_t last = str.find_last_not_of(" \t\n\r\f\v");
	if(first == std::string::npos || last == std::string::npos)
		return str;
	return str.substr(first, (last - first + 1));
}
//

void Socket::sendClientMessage(int clientFd, const std::string& message) {
	if (send(clientFd, message.c_str(), message.length(), 0) < 0){
		perror("Send failed");
		// optionally handle error e.g logging 
	}
}

void Socket::listChannels(int clientFd) {
	std::string channelList = "Channels:\n";
	for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
		channelList += it -> first + "\n";
	}
	send(clientFd, channelList.c_str(), channelList.length(), 0);
}

void Socket::processClientCommand(int clientFd, const std::string& receivedData) {
	//split the received data into commands and parameters
	std::istringstream iss(receivedData);
	std::vector<std::string> tokens{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};

	if (!tokens.empty()) {
		// handle command based on the first token
		const std::string& command = tokens[0];
		if (command == "commands") {
			const std::string commandsList = "Available commands:\ncreate <channelName>\njoin <channelName>\nleave <channelName>\nlist\n";
			send(clientFd, commandsList.c_str(), commandsList.length(), 0);
		} else if (command == "create" && tokens.size() == 2) {
			createChannel(clientFd, tokens[1]);
		} else if (command == "create") {
			sendClientMessage(clientFd, "Incorrect usage. Usage: create <channelName>");
		} else if (command == "join" && tokens.size() == 2) {
			joinChannel(clientFd, tokens[1]);
		} else if (command == "leave" && tokens.size() == 2) {
			leaveChannel(clientFd, tokens[1]);
		} else if (command == "list" && tokens.size() == 1) {
			listChannels(clientFd);
		} else {
			const char* errorMessage = "Uknown command or incorrect usage.\n";
			send(clientFd, errorMessage, strlen(errorMessage), 0);
		}
	}
}

void Socket::handleClientDisconnection(int clientIndex) {
	std::cout << "client "<<pfd[clientIndex].fd<<" disconnected." << std::endl;
	close(pfd[clientIndex].fd);
	pfd[clientIndex].fd = -1;
	ClientSockets.erase(std::remove(ClientSockets.begin(), ClientSockets.end(), pfd[clientIndex].fd), ClientSockets.end());
}

void Socket::handleReadError(int clientIndex) {
	std::cerr << "Error reading from client " << pfd[clientIndex].fd << std::endl;
	close(pfd[clientIndex].fd);
	pfd[clientIndex].fd = -1;
	ClientSockets.erase(std::remove(ClientSockets.begin(), ClientSockets.end(), pfd[clientIndex].fd), ClientSockets.end());
}


int Socket::Handle(int i) {
	char buff[BUFFER_SIZE];
	memset(buff, 0, BUFFER_SIZE); // to make sure the buffer is empty
	std::cout<<" Enter password" << std::endl;

	// Wait for data to be available on the socket
	fd_set read_fds;
	FD_ZERO(&read_fds);
	FD_SET(pfd[i].fd, &read_fds);
	select(pfd[i].fd + 1, &read_fds, NULL, NULL, NULL);

	int bytesRead = recv(pfd[i].fd, buff, BUFFER_SIZE - 1, 0); // -1 to leave space for the null terminator

	if (bytesRead > 0) {
		// Null-terminate the received data and convert it to a string
		buff[bytesRead] = '\0';
		std::string receivedData(buff);

		// Trim received data to remove leading and trailing spaces
		receivedData = trim(receivedData);

		if(!stats[i].Pass){
			//Assume first message after welcome is password
			stats[i].Pass = validatePassword(pfd[i].fd, receivedData);
			// send available commands to the user after validating password
			if(stats[i].Pass) {
				const char* commands = "Available commands:\ncreate <channelName>\njoin <channelName>\nleave <channelName>\nlist\n";
				send(pfd[i].fd, commands, strlen(commands), 0);
			}
		} else {
			// Hanlde other client commands
			processClientCommand(i, receivedData);
		}
	} else if (bytesRead == 0) {
		handleClientDisconnection(i);
	} else {
		handleReadError(i);
	}

	return bytesRead;
}

int	Socket::Welcome(int i){
	const char* WelcomeMessage = "Welcome to the IRC Server! Enter the Password\n ";

	stats[i].Chanel = 0;
	stats[i].Level = 0;
	stats[i].Pass = 0;
	int BytesSent = send(pfd[i].fd, WelcomeMessage, strlen(WelcomeMessage), 0);
	if (BytesSent < 0) {
		std::cerr << "Failed to send welcome message to the client." << std::endl;
	} else {
		std::cout << "Welcome message sent to the client." << std::endl;
	}
	Handle(i);
	return 1;
}


//
bool Socket::validatePassword(int clientFd, const std::string& receivedPassword) {
	std::cout << "Received password: " << receivedPassword << std::endl;
	if (receivedPassword == serverPassword) {
		const char*  successMessage = "Password accepted.\n";
		send(clientFd, successMessage, strlen(successMessage), 0);
		stats[clientFd].Pass = 1;
		return true;
	} else {
		const char* errorMessage = "Password incorrect.\n";
		send(clientFd, errorMessage, strlen(errorMessage), 0);
		return false;
	}
}


