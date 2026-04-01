#pragma once
#include "utils.h"
#include <vector>

class MemoryMonitor {
    public:
        MemoryMonitor();
        std::vector<MetricData> collectMetrics();
        bool initialize();
        double getMemoryUsage();
        Timer timer;
        void getTopMemoryProcesses();
    private:
        unsigned long long TotalMem;
        unsigned long long FreeMem;
        bool isFirstRead;
        bool readMemoryStats(unsigned long long &totalMem, unsigned long long &freeMem);
};