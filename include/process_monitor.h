#pragma once

class ProcessMonitor {
    public:
        ProcessMonitor();
        bool initialize();
        int getProcessCount();
        std::vector<MetricData> collectMetrics();
        bool getTopProcesses(std::vector<ProcessInfo>& topProcesses, int count = 5);
    private:
        bool isNumberDirectory(const std::string& name);
        bool readProcessMemeory(int pid, double& memoryMB, std::string& processName);
}