// reading /proc/net/dev
// calculating RX/TX bytes and rates

#include "network_monitor.h"
#include <fstream>

NetworkMonitor::NetworkMonitor(const std::string& iface="eth0") : interfaceName(iface), prevRxBytes(0), prevTxBytes(0), firstRead(true) {}

bool NetworkMonitor::initialize() {
    std::ifstream netDevFile("/proc/net/dev");
    if (!netDevFile.is_open()) {
        return false;
    }
    return true;
}

bool NetworkMonitor::readNetworkBytes(unsigned long long &rxBytes, unsigned long long &txBytes) {
    std::ifstream netDevFile("/proc/net/dev");
    if (!netDevFile.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(netDevFile, line)) {
        if (line.find(interfaceName + ":") != std::string::npos) {
            size_t colonPos = line.find(':');
            std::string data = line.substr(colonPos + 1);
            std::istringstream iss(data);
            iss >> rxBytes; 
            for (int i = 0; i < 8; ++i) {
                unsigned long long temp;
                iss >> temp;
            }
            iss >> txBytes; 
            return true;
        }
    }
    return false; 
}

std::vector<MetricData> NetworkMonitor::collectMetrics() {
    double rxMbps, txMbps;
    if (getNetworkUsage(rxMbps, txMbps)) {
        return {{"network_rx_mbps", rxMbps}, {"network_tx_mbps", txMbps}};
    }
    return {};
};

bool NetworkMonitor::getNetworkUsage(double& rxMbps, double& txMbps) {
    unsigned long long rxBytes, txBytes;
    if (!readNetworkBytes(rxBytes, txBytes)) {
        return false;
    }

    if (firstRead) {
        prevRxBytes = rxBytes;
        prevTxBytes = txBytes;
        firstRead = false;
        rxMbps = 0.0;
        txMbps = 0.0;
        return true;
    }

    unsigned long long rxDiff = rxBytes - prevRxBytes;
    unsigned long long txDiff = txBytes - prevTxBytes;

    rxMbps = (rxDiff * 8.0) / (1024 * 1024);
    txMbps = (txDiff * 8.0) / (1024 * 1024);

    prevRxBytes = rxBytes;
    prevTxBytes = txBytes;

    return true;
}