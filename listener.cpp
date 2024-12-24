#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

// Define color escape codes for purple
#define PURPLE "\x1b[35m"
#define RESET "\x1b[0m"

#define MAX_COMMAND_LENGTH 1024

// Global variables for LHOST (IP) and LPORT (port)
char LHOST[16]; // Store the IP address (IPv4)
int LPORT;      // Store the port number

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

// Function to print the confirmation of LHOST and LPORT settings
void print_confirmation() {
    printf("LHOST => %s\n", LHOST);
    printf("LPORT => %d\n", LPORT);
}

// Function to print the banner in purple
void print_banner() {
    printf(PURPLE);
    printf(" _                               _    \n");
    printf(" _ __ ___   ___| |_ ___ _ __ ___ _ __ __ _  ___| | __\n");
    printf("| '_ ` _ \\ / _ \\ __/ _ \\ '__/ __| '__/ _` |/ __| |/ /\n");
    printf("| | | | | |  __/ ||  __/ | | (__| | | (_| | (__|   < \n");
    printf("|_| |_| |_|\\___|\\__\\___|_|  \\___|_|  \\__,_|\\___|_\\_\\\n");
    printf("\n");
    printf(RESET); // Reset to default color
}

// Function to launch the listener (waiting for client connection)
void listener() {
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_size = sizeof(client_addr);
    char buffer[MAX_COMMAND_LENGTH];

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        exit(1);
    }

    // Prepare server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(LPORT);
    server_addr.sin_addr.s_addr = inet_addr(LHOST);

    // Bind the socket to the IP and port
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_socket, 1) == SOCKET_ERROR) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        exit(1);
    }

    // Print waiting for client message
    printf("Waiting for clients...\n");

    // Accept incoming connection
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed with error: %d\n", WSAGetLastError());
        exit(1);
    }

    // Print client connected
    printf("Client connected\n");

    // Handle the command input loop after the client is connected
    while (1) {
        printf("metercrack > ");
        fgets(buffer, MAX_COMMAND_LENGTH, stdin);

        // Exit condition for the listener loop (optional)
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Exiting...\n");
            break;
        }

        // Send command to the client (implement this part according to your needs)
        send(client_socket, buffer, strlen(buffer), 0);
    }

    // Close the client and server sockets
    closesocket(client_socket);
    closesocket(server_socket);
}

int main() {
    // Print the banner in purple
    print_banner();

    // Get LHOST (IP address) and LPORT (Port)
    printf("Enter LHOST (IP address): ");
    scanf("%s", LHOST);
    printf("Enter LPORT (Port number): ");
    scanf("%d", &LPORT);

    // Print the confirmation
    print_confirmation();

    // Initialize Winsock
    init_winsock();

    // Start the listener
    listener();

    // Clean up Winsock
    cleanup_winsock();

    return 0;
}
