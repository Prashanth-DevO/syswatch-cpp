// threshold comparison
// alert cooldown
// generating alert messages

#include "alert_engine.h"
#include <iostream>
#include <ctime>

AlertEngine::AlertEngine() : alertCooldown(300) {};
void AlertEngine::setThreshold(const std::string& metricType, double threshold) {
    thresholds[metricType] = threshold;
}
double AlertEngine::getThreshold(const std::string& metricType) {
    return thresholds[metricType];
}
bool AlertEngine::isThresholdBreached(const MetricData& metric) {
    return metric.value > thresholds[metric.type];
}
void AlertEngine::triggerAlert(const MetricData& metric) {
    if (canTriggerAlert(metric.type)) {
        std::string alertMessage = buildAlertMessage(metric);
        std::cout << alertMessage << std::endl;
        updateLastAlertTime(metric.type);
    }
}
bool AlertEngine::canTriggerAlert(const std::string& metricType) {
    auto now = std::time(nullptr);
    if (lastAlertTimes.find(metricType) == lastAlertTimes.end() ||
        difftime(now, lastAlertTimes[metricType]) >= alertCooldown) {
        return true;
    }
    return false;
}
void AlertEngine::updateLastAlertTime(const std::string& metricType) {
    lastAlertTimes[metricType] = std::time(nullptr);
}
std::string AlertEngine::buildAlertMessage(const MetricData& metric) {
    std::string message = "ALERT: " + metric.type + " usage at " + std::to_string(metric.value) + "% exceeds threshold of " + std::to_string(thresholds[metric.type]) + "%.";
    return message;
}  
