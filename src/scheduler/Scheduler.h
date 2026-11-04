#pragma once
#include <atomic>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include "process/Process.h"
#include "scheduler/CPUClock.h"
#include "scheduler/CPUCore.h"

// Owner: Karl
// Base class. FCFS and RR only differ in shouldPreempt().
//
// ONE scheduler thread runs the loop. Each tick:
//   1. clock.tick()
//   2. wake sleeping processes whose sleep counter reached 0
//   3. idle cores take the next process from the ready queue
//   4. every busy core runs one tick
//   5. finished -> release | sleeping -> release | preempt -> release + requeue
//   6. call the tick hook (OUTSIDE the lock)
//   7. short real-time pause so people can watch it
class Scheduler {
public:
    Scheduler(int numCores, uint64_t delayPerExec, CPUClock& clock);
    virtual ~Scheduler();

    // New process goes to the back of the ready queue. Thread-safe.
    void addProcess(std::shared_ptr<Process> process);

    void start();   // launches the scheduler thread
    void stop();    // stops and joins it (call before destroying anything else)
    bool isRunning() const;

    // Called every tick, with NO lock held (the hook may call addProcess()).
    // CommandHandler uses it to call ProcessGenerator::onTick().
    void setTickHook(std::function<void(uint64_t)> hook);

    // For screen -ls and report-util. Thread-safe.
    int getTotalCores() const;
    int getBusyCores() const;
    int getAvailableCores() const;
    double getUtilizationPercent() const;   // busy / total * 100

protected:
    // RR returns true when quantum is used up. FCFS always returns false.
    virtual bool shouldPreempt(const CPUCore& core) const = 0;

private:
    void run();        // the thread's loop
    void tickOnce();

    // One helper per step of tickOnce() (all assume the mutex is held).
    void wakeSleepers();
    void assignIdleCores();
    void runBusyCores();
    void releaseCores();
    int countBusyCores() const;

    int numCores;
    uint64_t delayPerExec;
    CPUClock& clock;
    std::vector<CPUCore> cores;
    std::deque<std::shared_ptr<Process>> readyQueue;
    std::vector<std::shared_ptr<Process>> sleepingList;
    std::function<void(uint64_t)> tickHook;
    mutable std::mutex mutex;
    std::thread worker;
    std::atomic<bool> running;
};
