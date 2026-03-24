// reading /proc/stat
// calculating CPU usage between two reads

#include "cpu_monitor.h"
#include <fstream>
#include <string>

CPUMonitor::CPUMonitor() prevTotalTime(0), prevIdlezTime(0), isFirstRead(true) {};

bool CPUMonitor::initialize() {
    std::ifstream file("/proc/stat");
    if(!file.is_open()) {
        return false;
    }
    return true;
}

double CPUMonitor::getCPUsage() {
    unsigned long long totalUser, idleTime;
    if (!readCPUStats(totalUser, idleTime)) {
        return -1.0;
    }
    if (isFirstRead) {
        prevTotalTime = totalUser;
        prevIdleTime = idleTime;
        isFirstRead = false;
        return 0.0;
    }
    unsigned long long totalDiff = totalUser - prevTotalTime;
    unsigned long long idleDiff = idleTime - prevIdleTime;

    double cpuUsage = (totalDiff > 0) ? (100.0 * (totalDiff - idleDiff) / totalDiff) : 0.0;

    prevTotalTime = totalUser;
    prevIdleTime = idleTime;

    return cpuUsage;
}

bool CPUMonitor::readCPUstats(unsigned long long &totalUser, unsigned long long &idleTime) {
    std::ifstream file("/proc/stat");
    if (!file.is_open()) {
        return false;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, 4) == "cpu ") {
            std::istringstream iss(line);
            std::string cpuLabel;
            unsigned long long user, nice, system, idle, iowait, irq, softirq;
            iss >> cpuLabel >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
            totalUser = user + nice + system + iowait + irq + softirq;
            idleTime = idle + iowait;
            return true;
        }
    }
    return false;
}

std::vector<MetricData> CPUMonitor::collectorMetrics() {
    std::vector<MetricData> metrics;
    double cpuUsage = getCPUUsage();
    if(cpuUsage>=0.0) {
        MetricData data;
        data.type = "cpu_usage";
        data.value = cpuUsage;
        data.timestamp = std::time(nullptr);
        metrics.push_back(data);
    }
    return metrics;
}