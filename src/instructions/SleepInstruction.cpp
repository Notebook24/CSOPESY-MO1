#include "instructions/SleepInstruction.h"
#include "process/Process.h"

SleepInstruction::SleepInstruction(uint8_t ticks) : ticks_(ticks) {}

void SleepInstruction::execute(Process& process) {
    process.sleepFor(ticks_);
}

std::string SleepInstruction::toString() const {
    return "SLEEP(" + std::to_string(static_cast<int>(ticks_)) + ")";
}