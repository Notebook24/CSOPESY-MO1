// Scheduler tests. Owner: Karl
// Each test builds a scheduler, adds processes BEFORE start(), and uses the
// tick hook to record what happened on every tick. Because the hook runs once
// per tick, the results are exact (no guessing with real-time sleeps).
// Tick numbers below assume: tick 1 is the first tick the scheduler runs.
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "instructions/Instruction.h"
#include "process/Process.h"
#include "scheduler/CPUClock.h"
#include "scheduler/FCFSScheduler.h"
#include "scheduler/RRScheduler.h"

// ---------- fake instructions ----------

class NoOpInstruction : public Instruction {
public:
    void execute(Process& process) override { (void)process; }
    std::string toString() const override { return "noop"; }
};

class FakeSleepInstruction : public Instruction {
public:
    FakeSleepInstruction(int ticks) : ticks(ticks) {}
    void execute(Process& process) override { process.sleepFor((uint8_t)ticks); }
    std::string toString() const override { return "sleep"; }
private:
    int ticks;
};

// ---------- tiny test helpers ----------

static int failures = 0;

static void checkEqual(const std::string& name, double actual, double expected) {
    if (actual == expected) {
        std::cout << "  PASS: " << name << std::endl;
    } else {
        std::cout << "  FAIL: " << name << " (expected " << expected
                  << ", got " << actual << ")" << std::endl;
        failures++;
    }
}

static void checkState(const std::string& name, ProcessState actual, ProcessState expected) {
    if (actual == expected) {
        std::cout << "  PASS: " << name << std::endl;
    } else {
        std::cout << "  FAIL: " << name << " (wrong state)" << std::endl;
        failures++;
    }
}

// A process with 'count' do-nothing instructions.
static std::shared_ptr<Process> makeProcess(int id, const std::string& name, int count) {
    std::vector<std::shared_ptr<Instruction>> program;
    for (int i = 0; i < count; i++) {
        program.push_back(std::make_shared<NoOpInstruction>());
    }
    return std::make_shared<Process>(id, name, program);
}

// noop, SLEEP(sleepTicks), noop
static std::shared_ptr<Process> makeSleepingProcess(int id, const std::string& name, int sleepTicks) {
    std::vector<std::shared_ptr<Instruction>> program;
    program.push_back(std::make_shared<NoOpInstruction>());
    program.push_back(std::make_shared<FakeSleepInstruction>(sleepTicks));
    program.push_back(std::make_shared<NoOpInstruction>());
    return std::make_shared<Process>(id, name, program);
}

// ---------- recorder: filled by the tick hook ----------

class Recorder {
public:
    Recorder() : scheduler(nullptr) {}

    void track(std::shared_ptr<Process> process) {
        tracked.push_back(process);
        states.push_back(std::vector<ProcessState>());
        finishTick.push_back(0);
    }

    // Runs at the end of every tick (scheduler thread).
    void onTick(uint64_t tick) {
        for (size_t i = 0; i < tracked.size(); i++) {
            states[i].push_back(tracked[i]->state());
            if (finishTick[i] == 0 && tracked[i]->isFinished()) {
                finishTick[i] = tick;
            }
        }
        busy.push_back(scheduler->getBusyCores());
        available.push_back(scheduler->getAvailableCores());
        utilization.push_back(scheduler->getUtilizationPercent());
    }

    // tick numbers start at 1, vectors start at 0
    ProcessState stateAt(size_t processIndex, size_t tick) const { return states[processIndex][tick - 1]; }
    int busyAt(size_t tick) const { return busy[tick - 1]; }
    int availableAt(size_t tick) const { return available[tick - 1]; }
    double utilizationAt(size_t tick) const { return utilization[tick - 1]; }

    Scheduler* scheduler;
    std::vector<std::shared_ptr<Process>> tracked;
    std::vector<uint64_t> finishTick;   // 0 = not finished yet

private:
    std::vector<std::vector<ProcessState>> states;
    std::vector<int> busy;
    std::vector<int> available;
    std::vector<double> utilization;
};

// Adds the tracked processes, runs until the clock reaches ticksToRun, stops.
static void runScenario(Scheduler& scheduler, CPUClock& clock, Recorder& recorder, uint64_t ticksToRun) {
    recorder.scheduler = &scheduler;
    for (size_t i = 0; i < recorder.tracked.size(); i++) {
        scheduler.addProcess(recorder.tracked[i]);
    }
    scheduler.setTickHook([&recorder](uint64_t tick) { recorder.onTick(tick); });
    scheduler.start();
    while (clock.getTicks() < ticksToRun) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    scheduler.stop();
}

// ---------- tests ----------

static void testFCFS() {
    std::cout << "FCFS: 1 core, p1 and p2 with 3 instructions each" << std::endl;
    CPUClock clock;
    FCFSScheduler scheduler(1, 0, clock);
    Recorder recorder;
    recorder.track(makeProcess(1, "p1", 3));
    recorder.track(makeProcess(2, "p2", 3));
    runScenario(scheduler, clock, recorder, 8);

    checkEqual("p1 finishes at tick 3", (double)recorder.finishTick[0], 3);
    checkEqual("p2 finishes at tick 6 (waited for p1)", (double)recorder.finishTick[1], 6);
}

static void testRR() {
    std::cout << "RR: 1 core, quantum 2, p1 has 4 instructions, p2 has 2" << std::endl;
    CPUClock clock;
    RRScheduler scheduler(1, 0, clock, 2);
    Recorder recorder;
    recorder.track(makeProcess(1, "p1", 4));
    recorder.track(makeProcess(2, "p2", 2));
    runScenario(scheduler, clock, recorder, 8);

    checkState("p1 is preempted (READY) at tick 2", recorder.stateAt(0, 2), ProcessState::READY);
    checkEqual("p2 finishes at tick 4", (double)recorder.finishTick[1], 4);
    checkEqual("p1 finishes at tick 6", (double)recorder.finishTick[0], 6);
}

static void testSleep() {
    std::cout << "SLEEP: noop, SLEEP(3), noop on 1 core" << std::endl;
    CPUClock clock;
    FCFSScheduler scheduler(1, 0, clock);
    Recorder recorder;
    recorder.track(makeSleepingProcess(1, "p1", 3));
    runScenario(scheduler, clock, recorder, 8);

    checkState("SLEEPING at tick 2 (left the core right away)", recorder.stateAt(0, 2), ProcessState::SLEEPING);
    checkState("still SLEEPING at tick 4", recorder.stateAt(0, 4), ProcessState::SLEEPING);
    checkEqual("core is free while it sleeps (tick 3)", recorder.busyAt(3), 0);
    checkEqual("finishes at tick 5", (double)recorder.finishTick[0], 5);
}

static void testDelay() {
    std::cout << "DELAY: delays-per-exec = 2, p1 has 3 instructions" << std::endl;
    CPUClock clock;
    FCFSScheduler scheduler(1, 2, clock);
    Recorder recorder;
    recorder.track(makeProcess(1, "p1", 3));
    runScenario(scheduler, clock, recorder, 10);

    // instructions run on ticks 1, 4, 7
    checkEqual("p1 finishes at tick 7", (double)recorder.finishTick[0], 7);
    checkEqual("core stays busy during delay (tick 2)", recorder.busyAt(2), 1);
}

static void testCoreCounts() {
    std::cout << "CORE COUNTS: 2 cores, 3 processes with 3 instructions each" << std::endl;
    CPUClock clock;
    FCFSScheduler scheduler(2, 0, clock);
    Recorder recorder;
    recorder.track(makeProcess(1, "p1", 3));
    recorder.track(makeProcess(2, "p2", 3));
    recorder.track(makeProcess(3, "p3", 3));
    runScenario(scheduler, clock, recorder, 8);

    checkEqual("tick 1: busy = 2", recorder.busyAt(1), 2);
    checkEqual("tick 1: available = 0", recorder.availableAt(1), 0);
    checkEqual("tick 1: utilization = 100", recorder.utilizationAt(1), 100.0);
    checkEqual("tick 4: busy = 1 (only p3 left)", recorder.busyAt(4), 1);
    checkEqual("tick 4: available = 1", recorder.availableAt(4), 1);
    checkEqual("tick 4: utilization = 50", recorder.utilizationAt(4), 50.0);
    checkEqual("p3 finishes at tick 6", (double)recorder.finishTick[2], 6);
}

int main() {
    testFCFS();
    testRR();
    testSleep();
    testDelay();
    testCoreCounts();

    if (failures == 0) {
        std::cout << "ALL TESTS PASSED" << std::endl;
        return 0;
    }
    std::cout << failures << " CHECK(S) FAILED" << std::endl;
    return 1;
}
