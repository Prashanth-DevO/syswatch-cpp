// calls all monitors
// passes data to alert engine
// prints output
// controls interval loop

#include "system_monitor.h";

SystemMonitor::SystemMonitor() :isRunning(false), intervalSeconds(5) {};
bool SystemMonitor::initialize() {
    // Initialize all monitors
    cpuMonitor.initialize();
    memoryMonitor.initialize();
    diskMonitor.initialize();
    networkMonitor.initialize();
    processMonitor.initialize();

    loadThresholdConfig("config/thresholds.conf");

    printStartupSummary();

    return true;
}

bool SystemMonitor::loadThresholdConfig(const std::string& filepath) {
    thresholds["cpu"] = 80.0; 
    thresholds["memory"] = 90.0; 
    thresholds["disk"] = 85.0; 
    thresholds["network"] = 1000.0; 
    thresholds["process"] = 100.0; 

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