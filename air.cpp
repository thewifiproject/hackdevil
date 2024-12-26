#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Funkce pro zpracování paketů
void packet_handler(unsigned char *user_data, const struct pcap_pkthdr *pkthdr, const unsigned char *packet) {
    // Detekce beacon frame (Wi-Fi)
    if (packet[0] == 0x80) {  // Beacon frame (frame control byte)
        printf("Beacon frame detected\n");

        // Zde můžete přidat kód pro analýzu BSSID, SSID atd.
    }

    // Detekce EAPOL handshakes
    if (packet[0] == 0x88 && packet[1] == 0x8e) {  // EAPOL (Ethernet frame type)
        printf("[HANDSHAKE] BSSID: %s\n", user_data);
    }
}

// Funkce pro inicializaci a začátek zachytávání
void start_capture(const char *interface, const char *bssid, const char *output_file) {
    pcap_t *handle;
    char errbuf[PCAP_ERRBUF_SIZE];

    // Otevření zařízení pro monitorovací režim
    handle = pcap_open_live(interface, 65536, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "pcap_open_live() failed: %s\n", errbuf);
        return;
    }

    // Nastavení callbacku pro zpracování paketů
    if (pcap_loop(handle, 0, packet_handler, (unsigned char *)bssid) < 0) {
        fprintf(stderr, "pcap_loop() failed: %s\n", pcap_geterr(handle));
        pcap_close(handle);
        return;
    }

    pcap_close(handle);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <bssid> <output file> <interface>\n", argv[0]);
        return 1;
    }

    // Zavolání funkce pro zachytávání paketů
    start_capture(argv[3], argv[1], argv[2]);

    return 0;
}
