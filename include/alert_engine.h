
#pragma once
#include <map>
#include <string>
#include <vector>
#include <ctime>
#include "utils.h"

class AlertEngine {
public:
    std::map<std::string, double> thresholds;

    AlertEngine();
    void setThreshold(const std::string& metricType, double threshold);
    double getThreshold(const std::string& metricName);
    bool isThresholdBreached(const MetricData& metric);
    void triggerAlert(const MetricData& metric);
    std::string buildAlertMessage(const MetricData& metric);
    void alertTopProcesses(const std::vector<CPUProcessInfo>& topCPUProcesses, double fromTimer);
    void alertTopProcesses(const std::vector<ProcessInfo>& topMemoryProcesses, double fromTimer);
    void clearTerminal();
    void printLoading(char symbol);
    void writeToCPUAlertLog(const std::vector<CPUProcessInfo>& topCPUProcesses, double fromTimer);
    void writeToMemoryAlertLog(const std::vector<ProcessInfo>& topMemoryProcesses, double fromTimer);
};