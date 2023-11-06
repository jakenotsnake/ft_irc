#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

const int MAX_CLIENTS = 10;
const int BUFFER_SIZE = 1024;
const int PORT = 6667;

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    std::vector<int> clientSockets;
    std::vector<struct pollfd> fds(MAX_CLIENTS + 1);

    // Create the server socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // Set up server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket to the specified IP and port
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        return 1;
    }

    // Start listening for client connections
    if (listen(serverSocket, MAX_CLIENTS) < 0) {
        std::cerr << "Failed to listen for connections" << std::endl;
        return 1;
    }

    std::cout << "IRC server listening on port " << PORT << std::endl;

    // Add server socket to the poll descriptor set
    fds[0].fd = serverSocket;
    fds[0].events = POLLIN;

    // Initialize client sockets
    for (int i = 1; i <= MAX_CLIENTS; ++i) {
        fds[i].fd = -1;
    }

    while (true) {
        // Call poll to check for events
        if (poll(&fds[0], MAX_CLIENTS + 1, -1) < 0) {
            std::cerr << "Failed to poll for events" << std::endl;
            return 1;
        }

        // Check if the server socket has an incoming connection
        if (fds[0].revents & POLLIN) {
            if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen)) < 0) {
                std::cerr << "Failed to accept connection" << std::endl;
                return 1;
            }

            std::string clientAddress = inet_ntoa(clientAddr.sin_addr);
            std::cout << "New client connected: " << clientAddress << std::endl;

            // Add the new client socket to the poll descriptor set
            for (int i = 1; i <= MAX_CLIENTS; ++i) {
                if (fds[i].fd == -1) {
                    fds[i].fd = clientSocket;
                    fds[i].events = POLLIN;
                    clientSockets.push_back(clientSocket);
                    break;
                }
            }
        }

        // Check for input from connected clients
        for (int i = 1; i <= MAX_CLIENTS; ++i) {
            if (fds[i].fd != -1 && (fds[i].revents & POLLIN)) {
                char buffer[BUFFER_SIZE];
                int bytesRead = recv(fds[i].fd, buffer, BUFFER_SIZE, 0);

                if (bytesRead <= 0) {
                    // Client closed the connection
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), fds[i].fd), clientSockets.end());
                } else {
                    // Process the received message
                    std::string message(buffer, bytesRead);
                    std::cout << "Received message from client: " << message << std::endl;
                }
            }
        }
    }

    // Close all client sockets
    for (std::vector<int>::const_iterator it = clientSockets.begin(); it != clientSockets.end(); ++it) {
        close(*it);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
