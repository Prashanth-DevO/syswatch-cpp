// reading /proc/stat
// calculating CPU usage between two reads

#include "cpu_monitor.h"
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <cctype>
#include <dirent.h>
#include <thread>
#include <unordered_map>

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

bool readProcessCpuStat(int pid, std::string& name, unsigned long long& cpuTime) {
    std::ifstream file("/proc/" + std::to_string(pid) + "/stat");
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    if (!std::getline(file, line)) {
        return false;
    }

    std::size_t l = line.find('(');
    std::size_t r = line.rfind(')');
    if (l == std::string::npos || r == std::string::npos || r <= l) {
        return false;
    }

    name = line.substr(l + 1, r - l - 1);
    std::istringstream iss(line.substr(r + 2));
    std::string field;
    unsigned long long utime = 0;
    unsigned long long stime = 0;
    for (int idx = 3; iss >> field; ++idx) {
        if (idx == 14) {
            utime = std::stoull(field);
        } else if (idx == 15) {
            stime = std::stoull(field);
            break;
        }
    }

    cpuTime = utime + stime;
    return true;
}
}

CPUMonitor::CPUMonitor() : prevTotalTime(0), prevIdleTime(0), isFirstRead(true) {}

bool CPUMonitor::initialize() {
    std::ifstream file("/proc/stat");
    if(!file.is_open()) {
        return false;
    }
    return true;
}

double CPUMonitor::getCPUUsage() {
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

bool CPUMonitor::readCPUStats(unsigned long long &totalUser, unsigned long long &idleTime) {
    std::ifstream file("/proc/stat");
    if (!file.is_open()) {
        return false;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, 4) == "cpu ") {
            std::istringstream iss(line);
            std::string cpuLabel;
            unsigned long long user, nice, system, idle, iowait, irq, softirq, steal = 0, guest = 0, guest_nice = 0;
            iss >> cpuLabel >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
            // Try to read optional fields if present
            iss >> steal >> guest >> guest_nice;
            totalUser = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
            idleTime = idle + iowait;
            return true;
        }
    }
    return false;
}

std::vector<MetricData> CPUMonitor::collectMetrics() {
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

void CPUMonitor::getTopCPUProcesses(std::vector<CPUProcessInfo>& topProcesses, double &fromTimer) {
    fromTimer = timer.elapsedSeconds();
    topProcesses.clear();

    unsigned long long total1 = 0, idle1 = 0, total2 = 0, idle2 = 0;
    if (!readCPUStats(total1, idle1)) {
        return;
    }

    std::unordered_map<int, unsigned long long> firstCpu;
    std::unordered_map<int, std::string> names;
    DIR* dir = opendir("/proc");
    if (!dir) {
        return;
    }
    for (dirent* entry = readdir(dir); entry != nullptr; entry = readdir(dir)) {
        std::string dirName = entry->d_name;
        if (entry->d_type != DT_DIR || !isPidDir(dirName)) {
            continue;
        }
        int pid = std::stoi(dirName);
        std::string name;
        unsigned long long cpuTime = 0;
        if (readProcessCpuStat(pid, name, cpuTime)) {
            firstCpu[pid] = cpuTime;
            names[pid] = name;
        }
    }
    closedir(dir);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    if (!readCPUStats(total2, idle2) || total2 <= total1) {
        return;
    }

    unsigned long long totalDelta = total2 - total1;
    std::vector<CPUProcessInfo> candidates;

    dir = opendir("/proc");
    if (!dir) {
        return;
    }
    for (dirent* entry = readdir(dir); entry != nullptr; entry = readdir(dir)) {
        std::string dirName = entry->d_name;
        if (entry->d_type != DT_DIR || !isPidDir(dirName)) {
            continue;
        }
        int pid = std::stoi(dirName);
        auto it = firstCpu.find(pid);
        if (it == firstCpu.end()) {
            continue;
        }
        std::string name;
        unsigned long long cpuTime = 0;
        if (!readProcessCpuStat(pid, name, cpuTime) || cpuTime < it->second) {
            continue;
        }

        double cpuPercent = (100.0 * static_cast<double>(cpuTime - it->second)) / static_cast<double>(totalDelta);
        candidates.push_back({name.empty() ? names[pid] : name, cpuPercent});
    }
    closedir(dir);

    const std::size_t topN = std::min<std::size_t>(5, candidates.size());
    if (topN == 0) {
        return;
    }

    std::partial_sort(candidates.begin(), candidates.begin() + topN, candidates.end(),
        [](const CPUProcessInfo& a, const CPUProcessInfo& b) {
            return a.cpuPercent > b.cpuPercent;
        });

    topProcesses.assign(candidates.begin(), candidates.begin() + topN);
}