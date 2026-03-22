#pragma once

struct MetricData {
    std::string type;
    double value;
    std::time_t timestamp;
};

struct Alert {
    std::string type;
    double value;
    std::string message;
    std::time_t timestamp;
};