#include "../incs/irc.hpp"

const int BUFFER_SIZE = 999;

//Constructor set easy variable;
Socket::Socket(){
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
	PassW = "HI";
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
				else
                	Handler(i, buffer);
        	}	
		}
	}
	return 1;
}
