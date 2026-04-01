
#pragma once
#include <map>
#include <string>
#include <vector>
#include <ctime>
#include "utils.h"

class AlertEngine {
public:
    std::map<std::string, double> thresholds;
    std::map<std::string, std::time_t> lastAlertTimes;
    int alertCooldown;

    AlertEngine();
    void setThreshold(const std::string& metricType, double threshold);
    double getThreshold(const std::string& metricName);
    bool isThresholdBreached(const MetricData& metric);
    void triggerAlert(const MetricData& metric);
    bool canTriggerAlert(const std::string& metricType);
    void updateLastAlertTime(const std::string& metricName);
    std::string buildAlertMessage(const MetricData& metric);
    void alertTopProcesses(const std::vector<CPUProcessInfo>& topCPUProcesses, double fromTimer);
    void alertTopProcesses(const std::vector<ProcessInfo>& topMemoryProcesses, double fromTimer);
};