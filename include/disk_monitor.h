#pragma once
#include "utils.h"
#include <vector>

class DiskMonitor{
    public:
       DiskMonitor(const std::string& mountPath = "/");
       std::vector<MetricData> collectMetrics();
       bool initialize();
       double getDiskUsage();
       std::string mountPath;
       bool printed;
    private:
       unsigned long long TotalBytes;
       unsigned long long freeBytes;
};