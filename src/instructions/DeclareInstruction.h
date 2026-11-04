#pragma once
#include <cstdint>
#include <string>
#include "instructions/Instruction.h"

// Owner: Alain
// DECLARE(var, value)
class DeclareInstruction : public Instruction {
public:
    DeclareInstruction(std::string variable, uint16_t value);

    void execute(Process& process) override;
    std::string toString() const override;

private:
    std::string variable_;
    uint16_t value_;
};