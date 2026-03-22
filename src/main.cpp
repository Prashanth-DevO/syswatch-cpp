#include "system_monitor.h"
#include <iostream>

int main() {
    SystemMonitor monitor;
    if(!monitor.initialize()) {
        std::cerr << "Failed to initialize system monitor." << std::endl;
        return 1;
    }
    monitor.startMonitoring();
    return 0;
}