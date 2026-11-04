#include "ProcessGenerator.h"

#include <iomanip>
#include <sstream>

#include "../instructions/InstructionFactory.h"

ProcessGenerator::ProcessGenerator(const Config& config, ProcessTable& table, ReadyCallback onNewProcess)
    : config_(config),
      table_(table),
      onNewProcess_(std::move(onNewProcess)),
      rng_(std::random_device{}()) {}

void ProcessGenerator::tick(uint64_t cpuTick) {
    if (!running_.load()) 
        return;

    if (config_.batchProcessFreq == 0 || cpuTick % config_.batchProcessFreq != 0) 
        return;

    std::string name;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        name = nextAutoName();
    }

    createProcess(name);
}

std::shared_ptr<Process> ProcessGenerator::createProcess(const std::string& name) {
    if (table_.exists(name)) 
        return nullptr;

    uint32_t id;
    uint64_t count;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        id = nextId_++;
        std::uniform_int_distribution<uint64_t> dist(config_.minIns, config_.maxIns);
        count = dist(rng_);
    }

    auto program = InstructionFactory::generateProgram(name, config_.minIns, config_.maxIns);   
    auto process = std::make_shared<Process>(id, name, std::move(program));

    if (!table_.add(process)) 
        return nullptr;   

    if (onNewProcess_) onNewProcess_(process);
        return process;
}

std::string ProcessGenerator::nextAutoName() {
    std::string name;
    
    do {
        std::ostringstream oss;
        oss << "p" << std::setw(2) << std::setfill('0') << ++autoCounter_;
        name = oss.str();
    } while (table_.exists(name));

    return name;
}