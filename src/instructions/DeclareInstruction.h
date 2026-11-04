#pragma once
#include "instructions/Instruction.h"

// DECLARE(var, value) - declares a uint16 variable with a default value.
class DeclareInstruction : public Instruction {
public:
    DeclareInstruction(std::string variable, uint16_t value);

    void execute(Process& process) override;
    InstructionType type() const override { return InstructionType::DECLARE; }
    std::string toString() const override;

private:
    std::string variable_;
    uint16_t value_;
};