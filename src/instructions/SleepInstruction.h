#pragma once
#include <cstdint>
#include <string>
#include "instructions/Instruction.h"

// Owner: Alain
// SLEEP(X): calls process.setSleepTicks(X). Does NOT change the state;
// the scheduler sees sleepTicks > 0 and takes the process off the core.
class SleepInstruction : public Instruction {
public:
    explicit SleepInstruction(uint8_t ticks);

    void execute(Process& process) override;
    std::string toString() const override;

private:
    uint8_t ticks_;
};