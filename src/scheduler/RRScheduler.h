#pragma once
#include "scheduler/Scheduler.h"

// Owner: Karl
class RRScheduler : public Scheduler {
public:
    RRScheduler(int numCores, uint64_t delayPerExec, CPUClock& clock, uint64_t quantumCycles);
protected:
    bool shouldPreempt(const CPUCore& core) const override;
private:
    uint64_t quantumCycles;
};
