#pragma once
#include <cstdint>
#include <string>

// Owner: Trisha
// Plain data. Fields are public on purpose (simple).
class Config {
public:
    Config();   // sets the defaults from the spec

    // Reads a space-separated file (e.g. "config.txt").
    // Returns false and fills errorMessage if the file is missing, a key is
    // missing, a value is not a number, or a value is out of range.
    bool loadFromFile(const std::string& path, std::string& errorMessage);

    int numCpu;                  // 1..128
    std::string scheduler;       // "fcfs" or "rr"
    uint64_t quantumCycles;      // 1..2^32
    uint64_t batchProcessFreq;   // 1..2^32
    uint64_t minIns;             // 1..2^32
    uint64_t maxIns;             // 1..2^32, must be >= minIns
    uint64_t delaysPerExec;      // 0..2^32
};
