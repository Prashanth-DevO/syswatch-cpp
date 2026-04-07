#pragma once
#include "utils.h"
#include <vector>

class TemperatureMonitor {
     public:
        TemperatureMonitor();
        std::vector<MetricData> collectMetrics();
        bool initialize();
    private:
        void readTemperatureStats(std::vector<MetricData>& metrics);
};