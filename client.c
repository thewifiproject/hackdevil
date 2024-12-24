#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

void error_exit(const char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void print_banner() {
    printf(" _ __ ___   ___| |_ ___ _ __ ___ _ __ __ _  ___| | __\n");
    printf("| '_ ` _ \\ / _ \\ __/ _ \\ '__/ __| '__/ _` |/ __| |/ /\n");
    printf("| | | | | |  __/ ||  __/ | | (__| | | (_| | (__|   <\n");
    printf("|_| |_| |_|\\___|\\__\\___|_|  \\___|_|  \\__,_|\\___|_\\_\\\n");
    printf("\n");
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int c;
    char lhost[16], buffer[1024];
    int lport;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        error_exit("Failed to initialize Winsock.");
    }

    // Print the banner
    print_banner();

    // Input LHOST and LPORT
    printf("ENTER LHOST: ");
    scanf("%15s", lhost);
    printf("ENTER LPORT: ");
    scanf("%d", &lport);

    // Create server socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        error_exit("Could not create socket.");
    }

    // Configure server address
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(lhost);
    server.sin_port = htons(lport);

    // Bind
    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        error_exit("Bind failed.");
    }
    printf("Listening on %s:%d\n", lhost, lport);

    // Listen
    if (listen(server_socket, 3) == SOCKET_ERROR) {
        error_exit("Listen failed.");
    }

    // Accept incoming connection
    c = sizeof(struct sockaddr_in);
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client, &c)) == INVALID_SOCKET) {
        error_exit("Accept failed.");
    }
    printf("Connection accepted.\n");

    // Send commands to the client and receive output
    while (1) {
        printf("Command> ");
        fgets(buffer, sizeof(buffer), stdin);
        send(client_socket, buffer, strlen(buffer), 0);  // Send command to client

        // Receive output from client
        int recv_size = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (recv_size == SOCKET_ERROR || recv_size == 0) {
            printf("Connection lost.\n");
            break;
        }
        buffer[recv_size] = '\0';
        printf("Client Response:\n%s\n", buffer);
    }

    // Cleanup
    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();

    return 0;
}
