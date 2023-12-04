#include "../incs/irc.hpp"

const int BUFFER_SIZE = 999;

//Constructor set easy variable;
Socket::Socket(const std::string &password, UserManager* um) : serverPassword(password), userManager(um) {
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

Socket::~Socket(){
    std::cout << "Closing Server" << std::endl;
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
                    stats[i].nickname = "client" + std::to_string(AcptSocket);
                    stats[i].fileDescriptor = AcptSocket;

                    // Add user to user manager
                    if (userManager != nullptr) {
                        std::string nickname = stats[i].nickname;
                        userManager->addUser(nickname, stats[i]);
                    }
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
    // channels.insert(std::make_pair(channelName, Channel(channelName, userManager)));
    if (userManager != nullptr) {
        channels.insert(std::make_pair(channelName, Channel(channelName, userManager)));

    } else {
        // Handle the error - UserManager is not initialized
        std::cerr << "UserManager is not initialized." << std::endl;
        return;
    }

    // get channel object
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    
    // set User who created as channel operator
    it->second.setChannelOperator(clientFd);
    // it->second.setOperatorStatus(getNickNameFromClientFd(clientFd), true);

    //Debug: channel creation success message
    std::cout << "Channel " << channelName << " created." << std::endl;
    sendClientMessage(clientFd, "Channel " + channelName + " created.\n Type 'join' " + channelName + " to join or type 'commands' to see available commands.\n");
}

Channel* Socket::getChannel(int clientFd) {
    for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->second.isUserInChannel(getNickNameFromClientFd(clientFd))) {
            std::cout << "found chanel" << std::endl;
            return &(it->second);
        }
    }
    return nullptr;

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
    std::string nickname = getNickNameFromClientFd(clientFd);
    if (it->second.addUser(nickname, clientFd))
        std::cout << " client " << clientFd << " joined channel " << channelName << std::endl;
    else
        std::cout << " client " << clientFd << " failed to joined channel " << channelName << std::endl;
}

void Socket::leaveChannel(int clientFd, const std::string &channelName) {
    // check if channel exists
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it == channels.end()) {
        std::cerr << "channel" << channelName << " does not exist" << std::endl ;
        return;
    }

    it->second.removeUser(stats[clientFd].nickname);
    std::cout<< " Client " << clientFd << " left channel " << channelName << std::endl;
    }


void Socket::listchannelUsers(int clientFd, const std::string& channelName) {
    // check if channel exists
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it == channels.end()) {
        std::cerr << "channel" << channelName << " does not exist" << std::endl ;
        return;
    }

    it->second.listUsers(clientFd);
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

    // check if clientFd is valid
    if (std::find(ClientSockets.begin(), ClientSockets.end(), clientFd) == ClientSockets.end()) {
       std::cerr << "Attempt to send on a closed or invalid socket: " << clientFd << std::endl;
    }
    // Fetch the nickname of the client
    std::string nickname = getNickNameFromClientFd(clientFd) + ": ";

    // prepend the nickname to the message
    std::string messageWithNickname = nickname + message;

    // send the message
    ssize_t bytesSent = send(clientFd, messageWithNickname.c_str(), messageWithNickname.length(), 0);
    if (bytesSent < 0) {
        std::cerr << "Error sending message to client " << clientFd << std::endl;
    } else {
        std::cout << "Message sent to client " << clientFd << std::endl;
    }
    
}

void Socket::listChannels(int clientFd) {
    std::string channelList = "Channels:\n";
    for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        channelList += it -> first + "\n";
    }
    send(clientFd, channelList.c_str(), channelList.length(), 0);
}

std::string Socket::getNickNameFromClientFd(int clientFd) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (pfd[i].fd == clientFd) {
            return stats[i].nickname;
        }
    }
    return "";
}





void Socket::setNickName(int clientFd, const std::string& nickname, int clientIndex) {
    std::string oldNickname = getNickNameFromClientFd(clientFd);

    // check if nickname is already taken
    if (userManager && userManager->getPfdStats(nickname)) {
        sendClientMessage(clientFd, "Nickname " + nickname + " is already taken.\n");
        return;
    }
    // update stats and userManager
    stats[clientIndex].nickname = nickname;
    if (userManager != nullptr) {
        //print old file descriptor
        std::cout << "old file descriptor: " << stats[clientIndex].getFileDescriptor() << std::endl;
        userManager->addUser(nickname, stats[clientIndex]);
        //print new file descriptor
        std::cout << "new file descriptor: " <<userManager->getPfdStats(nickname)->getFileDescriptor() << std::endl;
        userManager->removeUser(oldNickname);
    }

    const std::string successMessage = "Nickname set to " + nickname + "\n";
    sendClientMessage(clientFd, successMessage);
}

bool Socket::isRecognizedCommand(const std::string& input) {
    // list of recognized commands
    std::vector<std::string> commands{"join", "leave", "list", "setnickname", "create", "msg", "KICK", "INVITE"};
    for (const std::string& command : commands) {
        if (input.rfind(command, 0) == 0) { // check if input starts with the command
            return true;
        }
    }
    return false;
}


void Socket::processClientCommand(int clientFd, const std::string& receivedData, int clientIndex) {
    
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
        } else if (command == "setnickname" && tokens.size() == 2) {
            // log command processing
            // std::cout << "Processing 'setnickname' command with the nickname: " << tokens[1] << std::endl;
            setNickName(clientFd, tokens[1], clientIndex);
        } else if (command == "setnickname") {
            sendClientMessage(clientFd, "Incorrect usage. Usage: setnickname <nickname>\n");
        } else if (command == "create" && tokens.size() == 2) {
            //log command processing
            std::cout << "Processing 'create' command with the channel name: " << tokens[1] << std::endl;
            createChannel(clientFd, tokens[1]);
        } else if (command == "create") {
            sendClientMessage(clientFd, "Incorrect usage. Usage: create <channelName>\n");
        } else if (command == "join" && tokens.size() == 2) {
            joinChannel(clientFd, tokens[1]);
        } else if (command == "leave" && tokens.size() == 2) {
            leaveChannel(clientFd, tokens[1]);
        } else if (command == "list" && tokens.size() == 1) {
            listChannels(clientFd);
        } else if (command == "members" && tokens.size() == 2) {
            listchannelUsers(clientFd, tokens[1]);
        } else if (command == "setusername" && tokens.size() == 2) {
			UserName(clientFd, tokens[1]);
        } else if (command == "message" && tokens.size() > 1) {
			directMessage(clientFd, tokens);
        } else if (command == "topic" && tokens.size() > 1) {
			settopic1(clientFd, tokens);
        } else if (command == "KICK" && tokens.size() == 3) {
            // tokens[1] = channel name, tokens[2] = nickname
            std::string channelName = tokens[1];
            std::string nickname = tokens[2];

            // check if channel exists
            std::map<std::string, Channel>::iterator it = channels.find(channelName);
            if (it != channels.end()) {
                it->second.kickUser(clientFd, nickname);
            } else {
                std::cerr << "Channel " << channelName << " does not exist." << std::endl;
            }
        } else if (command == "INVITE" && tokens.size() == 3) {
            // tokens[1] = channel name, tokens[2] = nickname
            std::string channelName = tokens[1];
            std::string nickname = tokens[2];
            // check if channel exists
            std::map<std::string, Channel>::iterator it = channels.find(channelName);
            if (it != channels.end()) {
                it->second.inviteUser(clientFd, nickname);
            } else {
                std::cerr << "Channel " << channelName << " does not exist." << std::endl;
            }
        } else {
            // If the input is not a command treat is as a channel message
            if (!isRecognizedCommand(command)) {
                // get the channel the user is in
                Channel* channel = getChannel(clientFd);
                if (channel != nullptr) {
                    // send the message to the channel
                    channel->broadcastMessage(stats[clientIndex].getNickname(), receivedData);
                } else {
                    // send an error message to the client
                    const std::string errorMessage = "You are not in a channel. Use 'join' to join a channel.\n";
                    send(clientFd, errorMessage.c_str(), errorMessage.length(), 0);
                }
            } else {
                // send an error message to the client
                const std::string errorMessage = "Uknown command or incorrect usage.\n";
                send(clientFd, errorMessage.c_str(), errorMessage.length(), 0);
            }



            // const char* errorMessage = "Uknown command or incorrect usage.\n";
            // sendClientMessage(clientFd, errorMessage);
            
        }
    }
}

void Socket::handleClientDisconnection(int clientIndex) {
    std::cout << "client "<<pfd[clientIndex].fd<<" disconnected." << std::endl;
    close(pfd[clientIndex].fd);
    pfd[clientIndex].fd = -1;
    ClientSockets.erase(std::remove(ClientSockets.begin(), ClientSockets.end(), pfd[clientIndex].fd), ClientSockets.end());
    // Remove user from user manager
    if (userManager != nullptr) {
        userManager->removeUser(stats[clientIndex].nickname);
    }
}

void Socket::handleReadError(int clientIndex) {
    std::cerr << "Error reading from client " << pfd[clientIndex].fd << std::endl;
    close(pfd[clientIndex].fd);
    pfd[clientIndex].fd = -1;
    ClientSockets.erase(std::remove(ClientSockets.begin(), ClientSockets.end(), pfd[clientIndex].fd), ClientSockets.end());
    // Remove user from user manager
    if (userManager != nullptr) {
        userManager->removeUser(stats[clientIndex].nickname);
    }
}


int Socket::Handle(int i) {
    char buff[BUFFER_SIZE];
    memset(buff, 0, BUFFER_SIZE); // to make sure the buffer is empty

    

    // Debug: Indicate waiting for data
    std::cout<<" Waiting for data..." << std::endl;

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
            processClientCommand(pfd[i].fd, receivedData, i);
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

    if (receivedPassword == serverPassword) {
        const char*  successMessage = "Password accepted.\n Available commands:\n setnickname <nickname> \ncreate <channelName>\njoin <channelName>\nleave <channelName>\nlist\n";
        send(clientFd, successMessage, strlen(successMessage), 0);
        stats[i].Pass = true;

        return true;
    } else {
        const char* errorMessage = "Password incorrect.\n";
        send(clientFd, errorMessage, strlen(errorMessage), 0);
        return false;
    }
}

int Socket::directMessage(int clientfd, std::vector<std::string> Mes){
    for (int c = 1; c < 10; c++){
		if (Mes[1] == stats[c].UName){
			return DMessage(clientfd, c, Mes);
		}
	}
	send(clientfd, "User name does not exist\n", strlen("User name does not exist\n"), 0);
	return 0;
}

int	Socket::DMessage(int i, int c, std::vector<std::string> Mes){
	char *text = new char[stats[i].nickname.length() + 1];
	strcpy(text, stats[i].nickname.c_str());

	strcat(text, ":");

	for (int i = 1; i < (int)Mes.size(); i++){
		strcat(text, " ");
        strcat(text, Mes[i].c_str());
	}
	strcat(text, "\n");
	send(c, text, strlen(text), 0);
	return 1;
}

int Socket::UserName(int i, std::string Mes){
	Mes.erase(std::remove(Mes.begin(), Mes.end(), '\n'), Mes.cend());
	stats[i].UName = Mes;
	if (stats[i].nickname.empty())
		stats[i].nickname = Mes;
	const char* WelcomeMessage = "User Name set\n";
	std::cout << "Client No." << i << " set UserName to " << Mes << std::endl;
	send(i, WelcomeMessage, strlen(WelcomeMessage), 0);
	return 1;
}

int	Socket::settopic1(int clientFd, std::vector<std::string> Mes){
    char *text = new char[1000];
    for (int i = 1; i < (int)Mes.size(); i++){
        strcat(text, Mes[i].c_str());
        strcat(text, " ");
	}
	strcat(text, "\n");
    Channel* t = getChannel(clientFd);
    t->setTopic(clientFd, text);
    return 1;
}