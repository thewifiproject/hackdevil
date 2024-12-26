#include <pcap.h>
#include <iostream>
#include <string>
#include <cstring>
#include <iomanip>
#include <winsock2.h>

#define EAPOL_TYPE 0x88
#define WLAN_BEACON_TYPE 0x80

// Callback funkce pro zpracování paketů
void packet_handler(unsigned char *user_data, const struct pcap_pkthdr *pkthdr, const unsigned char *packet) {
    // Prohledáme pakety a zjistíme, zda je to Beacon nebo EAPOL

    // Získání typu rámce (frame type)
    unsigned char frame_type = packet[0] & 0x0C; // první byte obsahuje informace o typu rámce
    unsigned char bssid[6];
    memcpy(bssid, packet + 10, 6); // BSSID je obvykle na 10. pozici pro Beacon rámy

    // Detekce beaconů
    if (frame_type == WLAN_BEACON_TYPE) {
        std::cout << "[BEACON] BSSID: ";
        for (int i = 0; i < 6; i++) {
            printf("%02X", bssid[i]);
            if (i < 5) std::cout << ":";
        }
        std::cout << std::endl;
    }

    // Detekce EAPOL handshaku
    if (frame_type == EAPOL_TYPE) {
        std::cout << "[HANDSHAKE] BSSID: ";
        for (int i = 0; i < 6; i++) {
            printf("%02X", bssid[i]);
            if (i < 5) std::cout << ":";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: airhunter.exe <bssid> -w <output.pcap> <interface>" << std::endl;
        return 1;
    }

    // Získání jména rozhraní a souboru pro výstup
    std::string interface = argv[3];
    std::string output_file = argv[2];

    // Otevření zařízení pro monitorovací režim
    pcap_t *handle;
    char errbuf[PCAP_ERRBUF_SIZE];

    handle = pcap_open_live(interface.c_str(), 65536, 1, 1000, errbuf);
    if (handle == nullptr) {
        std::cerr << "Error opening interface " << interface << ": " << errbuf << std::endl;
        return 1;
    }

    // Filtr pro zachytávání pouze Beacon rámců
    struct bpf_program fp;
    std::string filter = "type mgt"; // Filtr na management rámce (beacon)
    if (pcap_compile(handle, &fp, filter.c_str(), 0, PCAP_NETMASK_UNKNOWN) == -1) {
        std::cerr << "Error compiling filter: " << pcap_geterr(handle) << std::endl;
        return 1;
    }
    if (pcap_setfilter(handle, &fp) == -1) {
        std::cerr << "Error setting filter: " << pcap_geterr(handle) << std::endl;
        return 1;
    }

    // Zápis do souboru pcap
    pcap_dumper_t *dumper = pcap_dump_open(handle, output_file.c_str());
    if (dumper == nullptr) {
        std::cerr << "Error opening dump file: " << pcap_geterr(handle) << std::endl;
        return 1;
    }

    // Zachytávání paketů
    std::cout << "Starting packet capture on " << interface << std::endl;
    pcap_loop(handle, 0, packet_handler, (unsigned char *)dumper);

    // Uzavření
    pcap_dump_close(dumper);
    pcap_close(handle);

    return 0;
}
