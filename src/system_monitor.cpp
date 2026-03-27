// calls all monitors
// passes data to alert engine
// prints output
// controls interval loop

#include "system_monitor.h"
#include <iostream>
#include <thread>
#include <chrono>

SystemMonitor::SystemMonitor() :isRunning(false), intervalSeconds(5) {};
bool SystemMonitor::initialize() {
    // Initialize all monitors
    if(!cpuMonitor.initialize() || !memoryMonitor.initialize() || !diskMonitor.initialize() || !networkMonitor.initialize()) {
        return false;
    }
    
    loadThresholdConfig("config/thresholds.conf");

    printStartupSummary();

    return true;
}

bool SystemMonitor::loadThresholdConfig(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open threshold config file: " << filepath << std::endl;
        return false;
    }
    const std::set<std::string> valid_keys = {
        "cpu_usage", "memory_usage", "disk_usage", "network_rx", "network_tx", "process_cpu", "process_memory"
    };
    int valid_count = 0;
    std::string line;
    while (std::getline(file, line)) {
        // Remove comments
        auto comment_pos = line.find('#');
        if (comment_pos != std::string::npos) line = line.substr(0, comment_pos);
        // Trim whitespace
        size_t first = line.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) continue;
        size_t last = line.find_last_not_of(" \t\n\r");
        line = line.substr(first, last - first + 1);
        if (line.empty()) continue;
        auto eq_pos = line.find('=');
        if (eq_pos == std::string::npos) continue;
        std::string key = line.substr(0, eq_pos);
        std::string value = line.substr(eq_pos + 1);
        // Trim key/value
        first = key.find_first_not_of(" \t\n\r");
        last = key.find_last_not_of(" \t\n\r");
        key = (first == std::string::npos) ? "" : key.substr(first, last - first + 1);
        first = value.find_first_not_of(" \t\n\r");
        last = value.find_last_not_of(" \t\n\r");
        value = (first == std::string::npos) ? "" : value.substr(first, last - first + 1);
        if (valid_keys.find(key) == valid_keys.end()) {
            std::cerr << "Warning: Invalid threshold key '" << key << "' in config. Skipping." << std::endl;
            continue;
        }
        try {
            double val = std::stod(value);
            thresholds[key] = val;
            valid_count++;
        } catch (...) {
            std::cerr << "Warning: Invalid value for key '" << key << "' in config. Skipping." << std::endl;
            continue;
        }
    }
    file.close();
    if (valid_count == 0) {
        std::cerr << "No valid thresholds found in config file." << std::endl;
        return false;
    }
    return true;
}

void SystemMonitor::startMonitoring() {
    isRunning = true;
    while (isRunning) {
        auto metrics = collectAllMetrics();
        processMetrics(metrics);
        std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
    }
}

void SystemMonitor::stopMonitoring() {
    isRunning = false;
}

void SystemMonitor::setMonitoringInterval(int seconds) {
    intervalSeconds = seconds;
}

void SystemMonitor::printStartupSummary() {
    logger.log("System Monitor initialized with the following thresholds:");
    for (const auto& threshold : thresholds) {
        logger.log(threshold.first + ": " + std::to_string(threshold.second));
    }
}

void SystemMonitor::processMetrics(const std::vector<MetricData>& metrics) {
    for (const auto& metric : metrics) {
        if (metric.value > thresholds[metric.type]) {
            alertEngine.triggerAlert(metric);
            logger.log("Alert triggered for " + metric.type + ": " + std::to_string(metric.value));
        }
    }
}

std::vector<MetricData> SystemMonitor::collectAllMetrics(){
    std::vector<MetricData> metrics;
    auto cpuData = cpuMonitor.collectMetrics();
    auto memoryData = memoryMonitor.collectMetrics();
    auto diskData = diskMonitor.collectMetrics();
    auto networkData = networkMonitor.collectMetrics();
    auto processData = processMonitor.collectMetrics();

    metrics.insert(metrics.end(), cpuData.begin(), cpuData.end());
    metrics.insert(metrics.end(), memoryData.begin(), memoryData.end());
    metrics.insert(metrics.end(), diskData.begin(), diskData.end());
    metrics.insert(metrics.end(), networkData.begin(), networkData.end());
    metrics.insert(metrics.end(), processData.begin(), processData.end());

    return metrics;
}