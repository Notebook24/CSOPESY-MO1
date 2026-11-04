#include "scheduler/Scheduler.h"
#include <chrono>

// Real-time pause after every tick so people can watch it run.
// Lower = faster emulator. Change this one number to tune the speed.
static const int TICK_PAUSE_MS = 10;

Scheduler::Scheduler(int numCores, uint64_t delayPerExec, CPUClock& clock)
    : numCores(numCores),
      delayPerExec(delayPerExec),
      clock(clock),
      running(false) {
    for (int i = 0; i < numCores; i++) {
        cores.push_back(CPUCore(i));
    }
}

Scheduler::~Scheduler() {
    // Safety net only. Callers must still call stop() BEFORE destroying
    // the derived scheduler, because the thread uses shouldPreempt().
    stop();
}

// ---------- public ----------

void Scheduler::addProcess(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(mutex);
    process->setState(ProcessState::READY);
    readyQueue.push_back(process);
}

void Scheduler::start() {
    if (running) {
        return;
    }
    running = true;
    worker = std::thread(&Scheduler::run, this);
}

void Scheduler::stop() {
    running = false;
    if (worker.joinable()) {
        worker.join();
    }
}

bool Scheduler::isRunning() const {
    return running;
}

void Scheduler::setTickHook(std::function<void(uint64_t)> hook) {
    std::lock_guard<std::mutex> lock(mutex);
    tickHook = hook;
}

int Scheduler::getTotalCores() const {
    return numCores;
}

int Scheduler::getBusyCores() const {
    std::lock_guard<std::mutex> lock(mutex);
    return countBusyCores();
}

int Scheduler::getAvailableCores() const {
    std::lock_guard<std::mutex> lock(mutex);
    return numCores - countBusyCores();
}

double Scheduler::getUtilizationPercent() const {
    std::lock_guard<std::mutex> lock(mutex);
    if (numCores == 0) {
        return 0.0;
    }
    double busy = (double)countBusyCores();
    double total = (double)numCores;
    return busy / total * 100.0;
}

// ---------- private: the loop ----------

void Scheduler::run() {
    while (running) {
        tickOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(TICK_PAUSE_MS));
    }
}

void Scheduler::tickOnce() {
    std::function<void(uint64_t)> hookCopy;
    uint64_t currentTick = 0;

    {
        std::lock_guard<std::mutex> lock(mutex);

        clock.tick();                 // step 1
        currentTick = clock.getTicks();

        wakeSleepers();               // step 2
        assignIdleCores();            // step 3
        runBusyCores();               // step 4
        releaseCores();               // step 5

        hookCopy = tickHook;          // copy it so we can call it unlocked
    }

    // step 6: NO lock held here, the hook may call addProcess()
    if (hookCopy) {
        hookCopy(currentTick);
    }
}

// ---------- private: one helper per step ----------

// Step 2
void Scheduler::wakeSleepers() {
    std::vector<std::shared_ptr<Process>> stillSleeping;

    for (size_t i = 0; i < sleepingList.size(); i++) {
        std::shared_ptr<Process> process = sleepingList[i];
        process->decrementSleepTicks();

        if (process->getSleepTicks() <= 0) {
            process->setState(ProcessState::READY);
            readyQueue.push_back(process);
        } else {
            stillSleeping.push_back(process);
        }
    }

    sleepingList = stillSleeping;
}

// Step 3
void Scheduler::assignIdleCores() {
    for (size_t i = 0; i < cores.size(); i++) {
        if (!cores[i].isIdle()) {
            continue;
        }
        if (readyQueue.empty()) {
            break;
        }

        std::shared_ptr<Process> process = readyQueue.front();
        readyQueue.pop_front();

        process->setState(ProcessState::RUNNING);
        process->setCoreId(cores[i].getId());
        cores[i].assign(process);
    }
}

// Step 4
void Scheduler::runBusyCores() {
    for (size_t i = 0; i < cores.size(); i++) {
        if (!cores[i].isIdle()) {
            cores[i].runTick(delayPerExec);
        }
    }
}

// Step 5
void Scheduler::releaseCores() {
    for (size_t i = 0; i < cores.size(); i++) {
        if (cores[i].isIdle()) {
            continue;
        }

        std::shared_ptr<Process> process = cores[i].getProcess();

        if (process->isFinished()) {
            // Process already marked itself FINISHED. Just free the core.
            cores[i].release();
        } else if (process->getSleepTicks() > 0) {
            // SLEEP ran this tick: leave the core right now.
            process->setState(ProcessState::SLEEPING);
            process->setCoreId(-1);
            cores[i].release();
            sleepingList.push_back(process);
        } else if (shouldPreempt(cores[i])) {
            // RR quantum used up: back to the end of the queue.
            process->setState(ProcessState::READY);
            process->setCoreId(-1);
            cores[i].release();
            readyQueue.push_back(process);
        }
    }
}

int Scheduler::countBusyCores() const {
    int busy = 0;
    for (size_t i = 0; i < cores.size(); i++) {
        if (!cores[i].isIdle()) {
            busy++;
        }
    }
    return busy;
}
