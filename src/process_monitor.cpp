// scanning /proc
// getting process name, PID
// optionally checking if a target process is alive
// top processes by memory or CPU later

#include "process_monitor.h"
#include <fstream>

ProcessMonitor::ProcessMonitor();

int ProcessMonitor::getProcessCount() {
    std::ifstream file("/proc");
    if (!file.is_open()) {
        return -1;
    }
    int count =0;
    std::string line;
    while (std::getline(file, line)) {
        if (isNumberDirectory(line)) {
            count++;
        }
    }
    file.close();
    return count;
}

std::vector<MetricData> ProcessMonitor::collectMetrics() {
    std::vector<MetricData> metrics;
    int processCount = getProcessCount();
    if (processCount >= 0) {
        metrics.push_back({"Process Count", static_cast<double>(processCount)});
    }
    return metrics;
}

bool ProcessMonitor::getTopProcesses(std::vector<ProcessInfo>& topProcesses, int count) {
    int maxMemory = 0;
    std::string bestProcess = "";
    std::ifstream file("/proc");
    if (!file.is_open()) {
        return false;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (isNumberDirectory(line)) {
            int pid = std::stoi(line);
            double memoryMB;
            std::string processName;
            if (readProcessMemory(pid, memoryMB, processName)) {
                if (memoryMB > maxMemory) {
                    maxMemory = memoryMB;
                    bestProcess = processName;
                }
            }
        }
    }
    file.close();
    return true;
}

bool ProcessMonitor::isNumberDirectory(const std::string& name) {
    for (char c : name) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool ProcessMonitor::readProcessMemory(int pid, double& memoryMB, std::string& processName) {
    std::ifstream file("/proc/" + std::to_string(pid) + "/status");
    if (!file.is_open()) {
        return false;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, 6) == "VmRSS:") {
            std::istringstream iss(line);
            std::string label;
            unsigned long memoryKB;
            iss >> label >> memoryKB;
            memoryMB = memoryKB / 1024.0;
        } else if (line.substr(0, 5) == "Name:") {
            std::istringstream iss(line);
            std::string label;
            iss >> label >> processName;
        }
    }
    file.close();
    return true;
}
