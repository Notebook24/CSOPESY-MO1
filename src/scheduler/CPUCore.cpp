#include "scheduler/CPUCore.h"

CPUCore::CPUCore(int id)
    : id(id), process(nullptr), quantumUsed(0), delayCounter(0) {
}

int CPUCore::getId() const {
    return id;
}

bool CPUCore::isIdle() const {
    return process == nullptr;
}

std::shared_ptr<Process> CPUCore::getProcess() const {
    return process;
}

void CPUCore::assign(std::shared_ptr<Process> newProcess) {
    process = newProcess;
    quantumUsed = 0;
    delayCounter = 0;
}

std::shared_ptr<Process> CPUCore::release() {
    std::shared_ptr<Process> released = process;
    process = nullptr;
    quantumUsed = 0;
    delayCounter = 0;
    return released;
}

void CPUCore::runTick(uint64_t delayPerExec) {
    if (process == nullptr) {
        return;
    }

    // Every tick the process holds the core counts toward the quantum,
    // including the busy-wait delay ticks.
    quantumUsed++;

    // Still busy-waiting from the last instruction: burn this tick.
    if (delayCounter > 0) {
        delayCounter--;
        return;
    }

    // Otherwise run one instruction, then start the delay.
    process->executeNext();
    delayCounter = delayPerExec;
}

uint64_t CPUCore::getQuantumUsed() const {
    return quantumUsed;
}
