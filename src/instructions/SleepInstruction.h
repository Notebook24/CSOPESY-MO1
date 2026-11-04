#pragma once
#include "instructions/Instruction.h"

// SLEEP(X) - sleeps the process for X (uint8) CPU ticks and relinquishes the CPU.
// The Process/Scheduler is responsible for counting the ticks down and re-queuing.
class SleepInstruction : public Instruction {
public:
    explicit SleepInstruction(uint8_t ticks);

    void execute(Process& process) override;
    InstructionType type() const override { return InstructionType::SLEEP; }
    std::string toString() const override;

private:
    uint8_t ticks_;
};