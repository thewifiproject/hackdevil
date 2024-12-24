#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")  // Link with the Winsock library

class MeterCrackListener {
public:
    void start() {
        std::string lhost, lport;

        // Set LHOST (IP address)
        std::cout << "Enter LHOST (IP address): ";
        std::cin >> lhost;
        std::cout << "LHOST => " << lhost << std::endl;

        // Set LPORT (Port)
        std::cout << "Enter LPORT (Port): ";
        std::cin >> lport;
        std::cout << "LPORT => " << lport << std::endl;

        // Confirm and auto-launch the listener
        std::cout << "Starting the listener with LHOST = " << lhost << " and LPORT = " << lport << "...\n";

        // Start the listener with network functionality
        listener(lhost, lport);
    }

    void listener(const std::string& lhost, const std::string& lport) {
        WSADATA wsaData;
        SOCKET listeningSocket = INVALID_SOCKET;
        struct sockaddr_in serverAddr;
        int port = std::stoi(lport);  // Convert string to int for the port

        // Initialize Winsock
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
            return;
        }

        // Create a socket for the listener
        listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listeningSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return;
        }

        // Set up the sockaddr_in structure
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(lhost.c_str());  // Convert IP address
        serverAddr.sin_port = htons(port);  // Convert port number to network byte order

        // Bind the socket to the IP address and port
        if (bind(listeningSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
            closesocket(listeningSocket);
            WSACleanup();
            return;
        }

        // Start listening for incoming connections
        if (listen(listeningSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
            closesocket(listeningSocket);
            WSACleanup();
            return;
        }

        std::cout << "Listener started at " << lhost << ":" << lport << std::endl;

        // Simulate the "metercrack" prompt after starting the listener
        metercrackPrompt();

        // Accept incoming connections (optional, not implemented fully here)
        SOCKET clientSocket;
        struct sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);

        while (true) {
            clientSocket = accept(listeningSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
            if (clientSocket == INVALID_SOCKET) {
                std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
                continue;
            }

            std::cout << "Connection established with client!" << std::endl;
            closesocket(clientSocket);  // For now, just close the client connection immediately
        }

        // Cleanup
        closesocket(listeningSocket);
        WSACleanup();
    }

    void metercrackPrompt() {
        std::string input;
        while (true) {
            std::cout << "metercrack > ";
            std::getline(std::cin, input);

            // Exit condition
            if (input == "exit") {
                break;
            }

            std::cout << "Executing command: " << input << std::endl;
            // Add more command handling here as necessary
        }
    }
};

int main() {
    MeterCrackListener listener;
    listener.start();
    return 0;
}
