#pragma once
#include "scheduler/Scheduler.h"

// Owner: Karl
class FCFSScheduler : public Scheduler {
public:
    FCFSScheduler(int numCores, uint64_t delayPerExec, CPUClock& clock);
protected:
    bool shouldPreempt(const CPUCore& core) const override;
};
