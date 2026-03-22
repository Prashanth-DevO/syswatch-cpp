#pragma once
#include <map>

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
};