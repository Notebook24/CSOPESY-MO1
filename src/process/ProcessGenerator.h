#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <random>
#include <string>

#include "../config/config.h"
#include "Process.h"
#include "ProcessTable.h"

class ProcessGenerator {
public:
    using ReadyCallback = std::function<void(std::shared_ptr<Process>)>;

    ProcessGenerator(const Config& config, ProcessTable& table, ReadyCallback onNewProcess);

    void start() { 
        running_.store(true); 
    }

    void stop()  { 
        running_.store(false); 
    }
    
    bool isRunning() const { 
        return running_.load(); 
    }

    void tick(uint64_t cpuTick);

    std::shared_ptr<Process> createProcess(const std::string& name);

private:
    std::string nextAutoName();   

    const Config& config_;
    ProcessTable& table_;
    ReadyCallback onNewProcess_;

    std::atomic<bool> running_{false};
    std::mutex mutex_;           
    std::mt19937_64 rng_;
    uint32_t nextId_ = 1;
    uint32_t autoCounter_ = 0;
};