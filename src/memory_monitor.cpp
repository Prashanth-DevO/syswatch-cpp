// reading /proc/meminfo
// computing memory usage percent

#include "memory_monitor.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <dirent.h>

namespace {
bool isPidDir(const std::string& name) {
    if (name.empty()) {
        return false;
    }
    for (char c : name) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

bool readProcessMemoryInfo(int pid, std::string& processName, double& memoryMB) {
    std::ifstream file("/proc/" + std::to_string(pid) + "/status");
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    bool hasName = false;
    bool hasMemory = false;
    while (std::getline(file, line)) {
        if (line.rfind("Name:", 0) == 0) {
            std::istringstream iss(line);
            std::string label;
            iss >> label >> processName;
            hasName = true;
        } else if (line.rfind("VmRSS:", 0) == 0) {
            std::istringstream iss(line);
            std::string label;
            unsigned long memoryKB = 0;
            iss >> label >> memoryKB;
            memoryMB = memoryKB / 1024.0;
            hasMemory = true;
        }
        if (hasName && hasMemory) {
            return true;
        }
    }

    return hasName && hasMemory;
}
}

MemoryMonitor::MemoryMonitor() : TotalMem(0), FreeMem(0), isFirstRead(true) {}

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

std::vector<MetricData> MemoryMonitor::collectMetrics() {
    double memUsage = getMemoryUsage();
    if(memUsage < 0) {
        return {};
    }
    return {{"memory_usage", memUsage}};
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
            totalMem = memKB * 1024; 
        } else if(line.substr(0, 8) == "MemFree:") {
            std::istringstream iss(line);
            std::string label;
            unsigned long long memKB;
            iss >> label >> memKB;
            freeMem = memKB * 1024; 
        }
    }
    return true;
}

void MemoryMonitor::getTopMemoryProcesses(std::vector<ProcessInfo>& topProcesses, double &fromTimer) {
    fromTimer = timer.elapsedSeconds();
    topProcesses.clear();

    DIR* dir = opendir("/proc");
    if (!dir) {
        return;
    }

    std::vector<ProcessInfo> candidates;
    for (dirent* entry = readdir(dir); entry != nullptr; entry = readdir(dir)) {
        std::string dirName = entry->d_name;
        if (entry->d_type != DT_DIR || !isPidDir(dirName)) {
            continue;
        }
        int pid = std::stoi(dirName);
        std::string name;
        double memoryMB = 0.0;
        if (readProcessMemoryInfo(pid, name, memoryMB)) {
            candidates.push_back({name, memoryMB});
        }
    }
    closedir(dir);

    const std::size_t topN = std::min<std::size_t>(5, candidates.size());
    if (topN == 0) {
        return;
    }

    std::partial_sort(candidates.begin(), candidates.begin() + topN, candidates.end(),
        [](const ProcessInfo& a, const ProcessInfo& b) {
            return a.memoryMB > b.memoryMB;
        });

    topProcesses.assign(candidates.begin(), candidates.begin() + topN);
}