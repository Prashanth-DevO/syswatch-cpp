#pragma once
#include "utils.h"

class NetworkMonitor {
    public:
        NetworkMonitor(const std::string& iface="eth0");
        std::string interfaceName;
        bool initialize();
        bool getNetworkUsage(double& rxMbps, double& txMbps);
        std::vector<MetricData> collectMetrics();
    private:
        unsigned long long prevRxBytes;
        unsigned long long prevTxBytes;
        bool firstRead;
        bool readNetworkBytes(unsigned long long& rxBytes, unsigned long long& txBytes);
};
