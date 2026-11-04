#pragma once
#include <cstdint>
#include <memory>
#include "process/Process.h"

// Owner: Karl
// A plain object (NOT a thread). The scheduler steps every core once per tick.
class CPUCore {
public:
    explicit CPUCore(int id);

    int getId() const;
    bool isIdle() const;
    std::shared_ptr<Process> getProcess() const;

    void assign(std::shared_ptr<Process> process);   // quantumUsed = 0
    std::shared_ptr<Process> release();              // returns it, core becomes idle

    // One tick of work. With delayPerExec = 0 this runs one instruction per tick.
    // With delayPerExec = N it runs one instruction, then busy-waits N ticks.
    void runTick(uint64_t delayPerExec);

    uint64_t getQuantumUsed() const;   // ticks the current process held this core

private:
    int id;
    std::shared_ptr<Process> process;
    uint64_t quantumUsed;
    uint64_t delayCounter;
};
