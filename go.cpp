#include <pcap.h>
#include <iostream>

void packetHandler(u_char *userData, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    std::cout << "Packet captured!" << std::endl;
    // Zde můžete analyzovat paket
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevs;
    pcap_t *handle;

    // Seznam síťových zařízení
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        std::cerr << "Error finding devices: " << errbuf << std::endl;
        return 1;
    }

    // Výběr první dostupné karty
    pcap_if_t *dev = alldevs;
    handle = pcap_open_live(dev->name, BUFSIZ, 1, 1000, errbuf); // Otevření zařízení pro zachytávání paketů
    if (handle == nullptr) {
        std::cerr << "Error opening device: " << errbuf << std::endl;
        return 1;
    }

    // Nastavení režimu monitorování
    if (pcap_set_rfmon(handle, 1) != 0) {
        std::cerr << "Error setting monitor mode" << std::endl;
        return 1;
    }

    // Zachytávání paketů
    if (pcap_loop(handle, 0, packetHandler, nullptr) < 0) {
        std::cerr << "Error capturing packets: " << pcap_geterr(handle) << std::endl;
        return 1;
    }

    pcap_close(handle); // Zavření handle
    return 0;
}
