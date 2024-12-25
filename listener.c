#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

void handleClient(SOCKET clientSocket) {
    char buffer[1024];
    int bytesReceived;

    while (1) {
        // Display prompt to enter a command
        printf("Shell> ");
        memset(buffer, 0, sizeof(buffer));

        // Get command input from user
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }

        // Send the command to the client
        send(clientSocket, buffer, strlen(buffer), 0);

        // Receive the output from the client
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
            printf("Connection closed or error receiving data.\n");
            break;
        }

        // Print the output
        buffer[bytesReceived] = '\0';
        printf("%s\n", buffer);
    }
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    char lhost[20], lport[10];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Winsock initialization failed\n");
        return 1;
    }

    // Get LHOST and LPORT from the user
    printf("Enter LHOST: ");
    scanf("%s", lhost);
    printf("Enter LPORT: ");
    scanf("%s", lport);

    // Create the socket
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    // Set up the server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(lhost);
    serverAddr.sin_port = htons(atoi(lport));

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Start listening
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        printf("Listen failed\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Listening...\n");

    // Accept a connection
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        printf("Accept failed\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Client connected\n");

    // Handle communication with the client
    handleClient(clientSocket);

    // Clean up
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
