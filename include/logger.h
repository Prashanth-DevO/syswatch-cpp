#pragma once
#include <string>
#include <fstream>
#include "utils.h"

class Logger {
    public:
      std::string logFilePath;
      std::ofstream logFile;

      Logger();
      bool initialize();
      void log(const std::string &message);
      void logMetric(const MetricData &metric);
      void logMetrics(const std::vector<MetricData> &metrics);
      void logError(const std::string &errorMessage);
      void logWarning(const std::string &warningMessage);
      void logInfo(const std::string &infoMessage);
      void close();
};