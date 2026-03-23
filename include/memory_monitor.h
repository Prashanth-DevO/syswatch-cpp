#pragma once
#include "utils.h"

class MemoryMonitor {
    public:
        MemoryMonitor();
        std::vector<MetricData> collectMetrics();
        bool initialize();
        double getMemoryUsage();
    private:
        unsigned long long TotalMem;
        unsigned long long FreeMem;
        bool isFirstRead;
        bool readMemoryStats(unsigned long long &totalMem, unsigned long long &freeMem);
}