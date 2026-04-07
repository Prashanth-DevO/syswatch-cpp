#include "temperature_monitor.h"
#include <filesystem>
#include <fstream>

TemperatureMonitor::TemperatureMonitor() {};

bool TemperatureMonitor::initialize() {
    std::string path = "/sys/class/thermal/";
    if(!std::filesystem::exists(path)||!std::filesystem::is_directory(path)) return false;
    return true;
}


void TemperatureMonitor::readTemperatureStats(std::vector<MetricData>& metrics) {
   std::string path = "/sys/class/thermal/";
   for(const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_directory() && entry.path().filename().string().find("thermal_zone") == 0) {
            std::ifstream tempFile(entry.path() / "temp");
            if (tempFile.is_open()) {
                double tempValue;
                tempFile >> tempValue;
                tempValue /= 1000.0; // Convert from millidegrees to degrees
                MetricData data;
                data.type = "temperature";
                data.value = tempValue;
                data.timestamp = std::time(nullptr);
                metrics.push_back(data);
            }
        }
    }
}

std::vector<MetricData> TemperatureMonitor::collectMetrics() {
    std::vector<MetricData> metrics;
    readTemperatureStats(metrics);
    return metrics;
}