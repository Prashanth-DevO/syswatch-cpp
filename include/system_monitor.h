#pragma once
#include "cpu_monitor.h"
#include "memory_monitor.h"
#include "disk_monitor.h"
#include "process_monitor.h"
#include "temperature_monitor.h"
#include "logger.h"
#include "alert_engine.h"
#include <map>
#include <vector>
#include <set>
#include <atomic>
#include <thread>

class SystemMonitor {
    public:
        CPUMonitor cpuMonitor;
        MemoryMonitor memoryMonitor;
        DiskMonitor diskMonitor;
        ProcessMonitor processMonitor;
        TemperatureMonitor temperatureMonitor;

        //support objects
         Logger logger;
         AlertEngine alertEngine;
         std::map<std::string, double> thresholds;
         bool isRunning;
         int intervalSeconds;
         int loadIndex;
         std::atomic<bool> loaderRunning;
         std::thread loaderThread;

         SystemMonitor();
         ~SystemMonitor();
         bool initialize();
         bool loadThresholdConfig(const std::string& filepath);
         void startMonitoring();
         void stopMonitoring();
         std::vector<MetricData> collectAllMetrics();
         void processMetrics(const std::vector<MetricData>& metrics);
         void setMonitoringInterval(int seconds);
         void printStartupSummary();
    private:
         void startLoader();
         void stopLoader();
};