#pragma once
#include "cpu_monitor.h"
#include "memory_monitor.h"
#include "disk_monitor.h"
#include "network_monitor.h"
#include "process_monitor.h"
#include "logger.h"
#include "alert_engine.h"
#include <map>
#include <vector>
#include <set>

class SystemMonitor {
    public:
        CPUMonitor cpuMonitor;
        MemoryMonitor memoryMonitor;
        DiskMonitor diskMonitor;
        NetworkMonitor networkMonitor;
        ProcessMonitor processMonitor;

        //support objects
         Logger logger;
         AlertEngine alertEngine;
         std::map<std::string, double> thresholds;
         bool isRunning;
         int intervalSeconds;

         SystemMonitor();
         bool initialize();
         bool loadThresholdConfig(const std::string& filepath);
         void startMonitoring();
         void stopMonitoring();
         std::vector<MetricData> collectAllMetrics();
         void processMetrics(const std::vector<MetricData>& metrics);
         void setMonitoringInterval(int seconds);
         void printStartupSummary();
};