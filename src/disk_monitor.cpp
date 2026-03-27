// checking disk usage using statvfs()

#include "disk_monitor.h"
#include <sys/statvfs.h>
#include <iostream>
#include <cstring>

DiskMonitor::DiskMonitor(const std::string& mountPath) : mountPath(mountPath), TotalBytes(0), freeBytes(0) {}

bool DiskMonitor::initialize() {
    struct statvfs stat;
    if (statvfs(mountPath.c_str(), &stat)!=0){
        std::cerr << "Failed to get disk stats for " << mountPath << ": " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

double DiskMonitor::getDiskUsage() {
    struct statvfs stat;
    if (statvfs(mountPath.c_str(), &stat) != 0) {
        std::cerr << "Failed to get disk stats for " << mountPath << ": " << strerror(errno) << std::endl;
        return -1.0;
    }
    TotalBytes = stat.f_blocks * stat.f_frsize; 
    freeBytes = stat.f_bfree * stat.f_frsize;  
    double usedBytes = TotalBytes - freeBytes;
    return (usedBytes / TotalBytes) * 100.0; 
}

std::vector<MetricData> DiskMonitor::collectMetrics() {
    std::vector<MetricData> metrics;
    double usage = getDiskUsage();
    if (usage >= 0) {
        metrics.push_back({"disk_usage_percent", usage});
    }
    return metrics;
}
