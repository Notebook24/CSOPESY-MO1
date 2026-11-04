#include "scheduler/FCFSScheduler.h"

FCFSScheduler::FCFSScheduler(int numCores, uint64_t delayPerExec, CPUClock& clock)
    : Scheduler(numCores, delayPerExec, clock) {
}

// FCFS never preempts: a process keeps its core until it finishes or sleeps.
bool FCFSScheduler::shouldPreempt(const CPUCore& core) const {
    (void)core;
    return false;
}
