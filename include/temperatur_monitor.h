#pragma once
#include "utils.h"

class TempratureMonitor {
     public:
        TemperatueMonitor();
        std:: vector<MetricData> collectorMEtrics();
        bool initialize();
    private:
        void readTemperatureStats(std::vector<MetricData>& metrics);
};