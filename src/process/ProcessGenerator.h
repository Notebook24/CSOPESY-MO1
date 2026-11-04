#pragma once
#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include "process/Process.h"
#include "process/ProcessTable.h"
#include "scheduler/Scheduler.h"

// Owner: Trisha
class ProcessGenerator {
public:
    ProcessGenerator(ProcessTable& table, Scheduler& scheduler,
                     uint64_t batchProcessFreq, uint64_t minIns, uint64_t maxIns);

    // Makes ONE process: new id, random program from InstructionFactory,
    // adds it to the table AND scheduler.addProcess().
    // Returns nullptr if the name already exists.
    // Used by "screen -s <name>" (Ryan) and by onTick() below.
    std::shared_ptr<Process> createProcess(const std::string& name);

    // scheduler-start / scheduler-stop
    void start();
    void stop();
    bool isRunning() const;

    // Called once per CPU tick by the scheduler's tick hook.
    // If running and currentTick is a multiple of batchProcessFreq,
    // creates the next auto-named process (p01, p02, ... p100).
    // Skips names that are already taken (user may have used "p01").
    void onTick(uint64_t currentTick);

private:
    ProcessTable& table;
    Scheduler& scheduler;
    uint64_t batchProcessFreq;
    uint64_t minIns;
    uint64_t maxIns;
    std::atomic<bool> running;
    int nextAutoNumber;
};
