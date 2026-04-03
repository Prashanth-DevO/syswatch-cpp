// writing metrics to CSV
// writing alerts to log file

#include "logger.h"
#include <stdexcept> // for runtime_error
#include <iostream> // for cerr
#include <fstream>  // for std::ios::app
#include <vector>

Logger::Logger() : logFilePath("logs/system_monitor.log") {};

void Logger::log(const std::string &message) {
    if (logFile.is_open()) {
        logFile << message << std::endl;
    }
}

bool Logger::initialize() {
    logFile.open(logFilePath, std::ios::app);
    if (!logFile) {
        throw std::runtime_error("Unable to open log file: " + logFilePath);
        return false;
    }
    log("Logger initialized.");
    return true;
}

void Logger::logMetric(const MetricData &metric) {
    log("Metric - " + metric.type + ": " + std::to_string(metric.value));
}

void Logger::logMetrics(const std::vector<MetricData> &metrics) {
    for (const auto& metric : metrics) {
        logMetric(metric);
    }
}

void Logger::logError(const std::string &errorMessage) {
    log("ERROR: " + errorMessage);
}

void Logger::logWarning(const std::string &warningMessage) {
    log("WARNING: " + warningMessage);
}

void Logger::logInfo(const std::string &infoMessage) {
    log("INFO: " + infoMessage);
}

void Logger::writeToCPUAlertLog(const std::vector<CPUProcessInfo>& topCPUProcesses, double fromTimer) {
    std::ofstream cpuAlertFile("logs/cpu_alerts.log", std::ios::app);
    if (cpuAlertFile.is_open()) {
        cpuAlertFile << "CPU Alert at " << std::time(nullptr) << " (collected in " << fromTimer << " seconds):" << std::endl;
        for (const auto& proc : topCPUProcesses) {
            cpuAlertFile << " - " << proc.name << ": " << proc.cpuPercent << "% CPU" << std::endl;
        }
    }
}

void Logger::writeToMemoryAlertLog(const std::vector<ProcessInfo>& topMemoryProcesses, double fromTimer) {
    std::ofstream memoryAlertFile("logs/memory_alerts.log", std::ios::app);
    if (memoryAlertFile.is_open()) {
        memoryAlertFile << "Memory Alert at " << std::time(nullptr) << " (collected in " << fromTimer << " seconds):" << std::endl;
        for (const auto& proc : topMemoryProcesses) {
            memoryAlertFile << " - " << proc.name << ": " << proc.memoryMB << " MB" << std::endl;
        }
    }
}

void Logger::close() {
    if (logFile.is_open()) {
        log("Logger shutting down.");
        logFile.close();
    }
}