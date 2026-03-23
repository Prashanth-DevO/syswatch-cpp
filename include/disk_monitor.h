#pragma once
#include "utils.h"

class DiskMonitor{
    public:
       DiskMonitor(const std::string& mountPath = "/");
       std::vector<MetricData> collectMetrics();
       bool initialize();
       double getDiskUsage();
       std::string mountPath;
    private:
       unsigned long long TotalBytes;
       unsigned long long freeBytes;
       bool readDiskStats(unsigned long long &totalRead, unsigned long long &totalWrite);
}