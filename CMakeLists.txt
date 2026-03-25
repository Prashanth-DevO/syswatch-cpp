cmake_minimum_required(VERSION 3.10)
project(syswatch-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(syswatch-cpp
    src/main.cpp
    src/system_monitor.cpp
    src/cpu_monitor.cpp
    src/memory_monitor.cpp
    src/disk_monitor.cpp
    src/network_monitor.cpp
    src/logger.cpp
    src/alert_engine.cpp
    src/process_monitor.cpp
    src/utils.cpp
)

target_include_directories(syswatch-cpp PRIVATE include)