#pragma once
#include "utils.h"
#include <vector>

class CPUMonitor {
    public:
        CPUMonitor();
        std::vector<MetricData> collectMetrics();
        bool initialize();
        double getCPUUsage();
        Timer timer;
        void getTopCPUProcesses();
    private:
        unsigned long long prevTotalTime;
        unsigned long long prevIdleTime;
        bool isFirstRead;
        bool readCPUStats(unsigned long long &totalUser, unsigned long long &idleTime);
};