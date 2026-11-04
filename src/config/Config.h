#pragma once        // prevents this header file from being included more than once.

#include <cstdint>
#include <string>


enum class SchedulerType {FCFS, RR};

struct Config {
    uint32_t numCpu = 4;
    SchedulerType scheduler = SchedulerType::RR;
    uint64_t quantumCycles = 5;
    uint64_t batchProcessFreq = 1;
    uint64_t minIns = 1000;
    uint64_t maxIns = 2000;
    uint64_t delayPerExec = 0; 
};

bool loadConfig(const std::string& path, Config& out, std::string& error);

std::string schedulerToString(SchedulerType s); // converts the scheduler enum into a readable string