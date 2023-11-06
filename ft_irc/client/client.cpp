#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

int main() {
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // Set up the server address
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(6667);  // Server port number
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Server IP address

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to connect to the server" << std::endl;
        return 1;
    }

    std::cout << "Connected to the server successfully" << std::endl;

    // Send and receive data
    char buffer[1024];
    while (true) {
        // Read user input from stdin
        std::cout << "Enter message to send (or 'quit' to exit): ";
        std::string input;
        std::getline(std::cin, input);

        // Check if user wants to quit
        if (input == "quit")
            break;

        // Send user input to the server
        ssize_t sentBytes = send(sockfd, input.c_str(), input.length(), 0);
        if (sentBytes <= 0) {
            std::cerr << "Failed to send data to the server" << std::endl;
            break;
        }

        // Receive response from the server
        ssize_t receivedBytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (receivedBytes <= 0) {
            std::cerr << "Failed to receive data from the server" << std::endl;
            break;
        }

        // Null-terminate the received data and print it
        buffer[receivedBytes] = '\0';
        std::cout << "Received: " << buffer << std::endl;
    }

    // Close the socket
    close(sockfd);

    return 0;
}