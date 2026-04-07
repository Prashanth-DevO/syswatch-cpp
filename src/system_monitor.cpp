// calls all monitors
// passes data to alert engine
// prints output
// controls interval loop

#include "system_monitor.h"
#include <iostream>
#include <thread>
#include <chrono>

SystemMonitor::SystemMonitor() :isRunning(false), intervalSeconds(5), loadIndex(0), loaderRunning(false) {};

SystemMonitor::~SystemMonitor() {
    stopLoader();
}

void SystemMonitor::startLoader() {
    if (loaderRunning) {
        return;
    }
    loaderRunning = true;
    loaderThread = std::thread([this]() {
        while (loaderRunning) {
            loadIndex = (loadIndex + 1) % 4;
            alertEngine.clearTerminal();
            alertEngine.printLoading(symbols[loadIndex]);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
}

void SystemMonitor::stopLoader() {
    if (!loaderRunning) {
        if (loaderThread.joinable()) {
            loaderThread.join();
        }
        return;
    }

    loaderRunning = false;
    if (loaderThread.joinable()) {
        loaderThread.join();
    }
    std::cout << "\r                  \r" << std::flush;
}

bool SystemMonitor::initialize() {
    // Initialize all monitors
    if(!cpuMonitor.initialize() || !memoryMonitor.initialize() || !diskMonitor.initialize()) {
        return false;
    }
    std::ifstream file("config/thresholds.conf");

    if(!file){
        std::cerr << "Failed to open threshold config file." << std::endl;
        return false;
    }
    
    loadThresholdConfig("config/thresholds.conf");
    // Copy thresholds to AlertEngine
    for (const auto& kv : thresholds) {
        alertEngine.setThreshold(kv.first, kv.second);
    }

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
        "cpu_usage", "memory_usage", "disk_usage", "process_cpu", "process_memory", "temperature"
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
        alertEngine.clearTerminal();
        startLoader();
        auto metrics = collectAllMetrics();
        processMetrics(metrics);

        for (int second = 0; second < intervalSeconds && isRunning; ++second) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        stopLoader();
    }
}

void SystemMonitor::stopMonitoring() {
    isRunning = false;
    stopLoader();
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
    bool hasAlert = false;
    for (const auto& metric : metrics) {
        if (metric.value > thresholds[metric.type]) {
            if (!hasAlert) {
                stopLoader();
                alertEngine.clearTerminal();
                hasAlert = true;
            }
            
            if(metric.type == "cpu_usage"){
                alertEngine.triggerAlert(metric);
                std::vector <CPUProcessInfo> topCPUProcesses;
                double fromTimer;
                if(!cpuMonitor.timer.isRunning) {
                    cpuMonitor.timer.start();
                }
                cpuMonitor.getTopCPUProcesses(topCPUProcesses, fromTimer);
                alertEngine.alertTopProcesses(topCPUProcesses, fromTimer);
            }
            else if(metric.type == "memory_usage"){
                alertEngine.triggerAlert(metric);
                std::vector <ProcessInfo> topMemoryProcesses;
                double fromTimer;
                if(!memoryMonitor.timer.isRunning) {
                    memoryMonitor.timer.start();
                }
                memoryMonitor.getTopMemoryProcesses(topMemoryProcesses, fromTimer);
                alertEngine.alertTopProcesses(topMemoryProcesses, fromTimer);
            }
            else if(metric.type == "disk_usage"){
                alertEngine.triggerAlert(metric);
                diskMonitor.printed = true;
            }
            else if(metric.type == "temperature"){
                alertEngine.triggerAlert(metric);
            }
            else{
                alertEngine.triggerAlert(metric);
            }   
        }
        else{
            if(metric.type == "cpu_usage" && cpuMonitor.timer.isRunning) {
                cpuMonitor.timer.isRunning = false;
            }
            if(metric.type == "memory_usage" && memoryMonitor.timer.isRunning) {
                memoryMonitor.timer.isRunning = false;
            }
        }
    }
}

std::vector<MetricData> SystemMonitor::collectAllMetrics(){
    std::vector<MetricData> metrics;
    auto cpuData = cpuMonitor.collectMetrics();
    auto memoryData = memoryMonitor.collectMetrics();
    auto diskData = diskMonitor.collectMetrics();
    auto temperatureData = temperatureMonitor.collectMetrics();
    auto processData = processMonitor.collectMetrics();


    metrics.insert(metrics.end(), cpuData.begin(), cpuData.end());
    metrics.insert(metrics.end(), memoryData.begin(), memoryData.end());
    metrics.insert(metrics.end(), diskData.begin(), diskData.end());
    metrics.insert(metrics.end(), temperatureData.begin(), temperatureData.end());
    metrics.insert(metrics.end(), processData.begin(), processData.end());

    return metrics;
}