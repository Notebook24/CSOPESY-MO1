#include "scheduler/RRScheduler.h"

RRScheduler::RRScheduler(int numCores, uint64_t delayPerExec, CPUClock& clock,
                         uint64_t quantumCycles)
    : Scheduler(numCores, delayPerExec, clock),
      quantumCycles(quantumCycles) {
}

// RR preempts once the process has held the core for a full quantum.
bool RRScheduler::shouldPreempt(const CPUCore& core) const {
    return core.getQuantumUsed() >= quantumCycles;
}
