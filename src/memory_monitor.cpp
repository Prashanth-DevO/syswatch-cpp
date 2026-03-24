// reading /proc/meminfo
// computing memory usage percent

#include "memory_monitor.h"
#include <fstream>

MemoryMonitor::MemoryMonitor() totalBytes(0), freeBytes(0) {};

bool MemoryMonitor::initialize() {
    std::ifstream file("/proc/meminfo");
    if(!file.is_open()) {
        return false;
    }
    return true;
}

double MemoryMonitor::getMemoryUsage() {
    unsigned long long totalMem , freeMem;
    if(!readMemoryStats(totalMem, freeMem)) {
        return -1.0;
    }
    if(isFirstRead) {
        TotalMem = totalMem;
        FreeMem = freeMem;
        isFirstRead = false;
        return 0.0;
    }
    double memUsage = (TotalMem > 0) ? (100.0 * (TotalMem - FreeMem) / TotalMem) : 0.0;
    TotalMem = totalMem;
    FreeMem = freeMem;
    return memUsage;
}

vector<MetricData> MemoryMonitor::collectMetrics() {
    double memUsage = getMemoryUsage();
    if(memUsage < 0) {
        return {};
    }
    return {{"Memory Usage", memUsage}};
}

bool MemoryMonitor::readMemoryStats(unsigned long long &totalMem, unsigned long long &freeMem) {
    std::ifstream file("/proc/meminfo");
    if(!file.is_open()) {
        return false;
    }
    std::string line;
    while(std::getline(file, line)) {
        if(line.substr(0, 9) == "MemTotal:") {
            std::istringstream iss(line);
            std::string label;
            unsigned long long memKB;
            iss >> label >> memKB;
            totalMem = memKB * 1024; // Convert to bytes
        } else if(line.substr(0, 8) == "MemFree:") {
            std::istringstream iss(line);
            std::string label;
            unsigned long long memKB;
            iss >> label >> memKB;
            freeMem = memKB * 1024; // Convert to bytes
        }
    }
    return true;
}