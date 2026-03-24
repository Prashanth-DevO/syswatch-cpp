#pragma once
#include "utils.h"

class CPUMonitor {
    public:
        CPUMonitor();
        std::vector<MetricData> collectMetrics();
        bool initialize();
        double getCPUUsage();
    private:
        unsigned long long prevTotalTime;
        unsigned long long prevIdleTime;
        bool isFirstRead;
        bool readCPUStats(unsigned long long &totalUser, unsigned long long &idleTime);
};