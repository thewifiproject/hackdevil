#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUFFER_SIZE 1024

// Function to initialize Winsock
int init_winsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed with error: %d\n", result);
        return 0;
    }
    return 1;
}

// Function to handle setting LHOST and LPORT
void set_configuration(char* config_name, char* config_value) {
    printf("%s => %s\n", config_name, config_value);
}

int main() {
    char lhost[50], lport[10];
    SOCKET server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Initialize Winsock
    if (!init_winsock()) {
        return 1;
    }

    // Set LHOST and LPORT
    printf("Enter LHOST (IP address): ");
    fgets(lhost, sizeof(lhost), stdin);
    lhost[strcspn(lhost, "\n")] = 0;  // Remove newline character

    set_configuration("LHOST", lhost);

    printf("Enter LPORT (Port): ");
    fgets(lport, sizeof(lport), stdin);
    lport[strcspn(lport, "\n")] = 0;  // Remove newline character

    set_configuration("LPORT", lport);

    // Create server socket
    server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_sock == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(lport));
    server_addr.sin_addr.s_addr = inet_addr(lhost);

    // Bind socket
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        closesocket(server_sock);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_sock, 1) == SOCKET_ERROR) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        closesocket(server_sock);
        WSACleanup();
        return 1;
    }

    printf("Waiting for clients...\n");

    // Accept a client connection
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
    if (client_sock == INVALID_SOCKET) {
        printf("Accept failed with error: %d\n", WSAGetLastError());
        closesocket(server_sock);
        WSACleanup();
        return 1;
    }

    // Once client is connected, print command input prompt
    printf("Client connected. Command input:\n");
    
    // Main interaction loop after client connection
    while (1) {
        printf("metercrack > ");
        memset(buffer, 0, sizeof(buffer));

        // Get command input from user
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline character

        // Send command to client (if any)
        if (strlen(buffer) > 0) {
            send(client_sock, buffer, strlen(buffer), 0);
        }

        // Receive response from client
        int recv_size = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (recv_size == SOCKET_ERROR) {
            printf("Recv failed with error: %d\n", WSAGetLastError());
            break;
        }
        buffer[recv_size] = '\0';  // Null-terminate received data

        if (recv_size > 0) {
            printf("Client response: %s\n", buffer);
        }
    }

    // Cleanup
    closesocket(client_sock);
    closesocket(server_sock);
    WSACleanup();
    
    return 0;
}
