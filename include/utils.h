#pragma once
#include <string>
#include <ctime> 

struct MetricData {
    std::string type;
    double value;
    std::time_t timestamp;
};

struct Alert {
    std::string type;
    double value;
    std::string message;
    std::time_t timestamp;
};

struct ProcessInfo {
    std::string name;
    double memoryMB;
};

struct CPUProcessInfo {
    std::string name;
    double cpuPercent;
};


struct Timer {
    std::time_t startTime;
    bool isRunning;

    void start() {
        startTime = std::time(nullptr);
        isRunning = true;
    }

    double elapsedSeconds() {
        return difftime(std::time(nullptr) , startTime);
    }
}