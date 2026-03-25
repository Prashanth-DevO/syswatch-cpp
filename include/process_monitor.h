#pragma once
#include "utils.h"
#include <vector>

class ProcessMonitor {
    public:
        ProcessMonitor();
        int getProcessCount();
        std::vector<MetricData> collectMetrics();
        bool getTopProcesses(std::vector<ProcessInfo>& topProcesses, int count = 5);
    private:
        bool isNumberDirectory(const std::string& name);
        bool readProcessMemory(int pid, double& memoryMB, std::string& processName);
};