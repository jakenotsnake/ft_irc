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

///

bool isValidFd(int fd) {
    return fcntl(fd, F_GETFL) != -1 || errno != EBADF;
}

int Socket::Polling() {
	while(true) {
		PollRet = poll(pfd, MAX_CLIENTS + 1, (60 * 1000)); // 60 seconds
		if (PollRet < 0 ) {
			perror("Poll() failed");
			exit(EXIT_FAILURE);
		}

		// New connection handling
		if(pfd[0].revents & POLLIN) {
			// Accept incoming connection
			if ((AcptSocket = accept(pfd[0].fd, (struct sockaddr *)&clientAddr, (socklen_t *)&AdrsLen)) >= 0) {
				std::cout << "Accepted new client, socket fd: " << AcptSocket << std::endl;
			} else {
				std::cerr << "Failed to accept client, error: " << strerror(errno) << std::endl;
			}
			// Log IP address of client
			std::string clientAddress = inet_ntoa(clientAddr.sin_addr);
			std::cout << "New client connected: " << clientAddress << std::endl;
			// Send welcome message to the newly connected client
			const char* WelcomeMessage = "Welcome to the IRC Server! Enter the Password\n ";
			if(send(AcptSocket, WelcomeMessage, strlen(WelcomeMessage), 0) < 0) {
				perror("Send failed in Polling()a");
			}
			// Find available pfd socket
			for (int i = 1; i <= MAX_CLIENTS; ++i) {
				if (pfd[i].fd == -1) {
					pfd[i].fd = AcptSocket;
					pfd[i].events = POLLIN;
					ClientSockets.push_back(AcptSocket);

					// initialize stats for the new connection
					stats[i].Pass = false;
					stats[i].Chanel = 0;
					stats[i].Level = 0;
					break;
				}
			}
		}
		// Data handling for established connections
		for (int i = 1; i <= MAX_CLIENTS; ++i) {
			if (pfd[i].fd != -1 && (pfd[i].revents & POLLIN)) {
				Handle(i); // Process the data from client
			}
		}
	}
}


void Socket::createChannel(int clientFd, const std::string &channelName) {
	//check if channel exists
	if (channels.find(channelName) != channels.end()) {
		sendClientMessage(clientFd, "Channel " + channelName + " already exists.\n");
		return;
	}
	// Debug: log channel creation
	std::cout << "Creating channel " << channelName << std::endl;

	// create a new channel
	channels.insert(std::make_pair(channelName, Channel(channelName)));

	//Debug: channel creation success message
	std::cout << "Channel " << channelName << " created." << std::endl;
	sendClientMessage(clientFd, "Channel " + channelName + " created.\n Type 'join' " + channelName + " to join or type 'commands' to see available commands.\n");
	// std::cout << "Channel " << channelName << " created." << std::endl;
	// const std::string successMessage = "Channel " + channelName + " created.\n";
	// send(clientFd, successMessage.c_str(), successMessage.length(), 0);
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

	std::cout << "Attempting to send message to client " << clientFd << std::endl;

	if (std::find(ClientSockets.begin(), ClientSockets.end(), clientFd) != ClientSockets.end()) {
		ssize_t bytesSent = send(clientFd, message.c_str(), message.length(), 0);
		if (bytesSent < 0) {
			perror("Send failed in SendClientMessage");
			std::cerr << "Error code: " << errno << "Error message: " << strerror(errno) << std::endl;
		}
	} else {
		std::cerr << "Attempt to send on a closed or invalid socket: " << clientFd << std::endl;
		return;
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
	
	//log the received data
	std::cout << "Received data: " << receivedData << std::endl;


	//split the received data into commands and parameters
	std::istringstream iss(receivedData);
	std::vector<std::string> tokens{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};

	if (!tokens.empty()) {
		// handle command based on the first token
		const std::string& command = tokens[0];
		// log the parsed command
		std::cout << "Parsed command: " << command << std::endl;

		if (command == "commands" && tokens.size() == 1) {
			const std::string commandsList = "Available commands:\ncreate <channelName>\njoin <channelName>\nleave <channelName>\nlist\n";
			send(clientFd, commandsList.c_str(), commandsList.length(), 0);
		} else if (command == "create" && tokens.size() == 2) {
			//log command processing
			std::cout << "Processing 'create' command with the channel name: " << tokens[1] << std::endl;
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

	

	// Debug: Indicate waiting for data
	std::cout<<" Waiting for data..." << std::endl;

	// Wait for data to be available on the socket
	// fd_set read_fds;
	// FD_ZERO(&read_fds);
	// FD_SET(pfd[i].fd, &read_fds);
	// select(pfd[i].fd + 1, &read_fds, NULL, NULL, NULL);

	int bytesRead = recv(pfd[i].fd, buff, BUFFER_SIZE - 1, 0); // -1 to leave space for the null terminator

	if (bytesRead > 0) {
		// Null-terminate the received data and convert it to a string
		buff[bytesRead] = '\0';
		std::string receivedData(buff);
		// Debug: Log received data
		std::cout << "Raw data received from client " << pfd[i].fd << ": " << receivedData << std::endl;

		// Trim received data to remove leading and trailing spaces
		receivedData = trim(receivedData);


		if (!stats[i].Pass) {
			// Validate password
			validatePassword(i, pfd[i].fd, receivedData);
		} else {
			// Process commands after successful password entry
			processClientCommand(pfd[i].fd, receivedData);
		}
	} else if (bytesRead == 0) {
		handleClientDisconnection(i);
	} else {
		handleReadError(i);
	}

	return bytesRead;
}



//
bool Socket::validatePassword(int i, int clientFd, const std::string& receivedPassword) {
	// const char* WelcomeMessage = "Welcome to the IRC Server! Enter the Password\n ";
	// send(clientFd, WelcomeMessage, strlen(WelcomeMessage), 0);
	// int BytesSent = send(pfd[i].fd, WelcomeMessage, strlen(WelcomeMessage), 0);

	if (receivedPassword == serverPassword) {
		const char*  successMessage = "Password accepted.\n Available commands:\ncreate <channelName>\njoin <channelName>\nleave <channelName>\nlist\n";
		send(clientFd, successMessage, strlen(successMessage), 0);
		stats[i].Pass = true;

		return true;
	} else {
		const char* errorMessage = "Password incorrect.\n";
		send(clientFd, errorMessage, strlen(errorMessage), 0);
		return false;
	}
}


