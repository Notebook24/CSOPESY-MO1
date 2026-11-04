#pragma once
#include "instructions/Instruction.h"

// SUBTRACT(var1, var2/value, var3/value) -> var1 = var2 - var3 (clamped at 0).
class SubtractInstruction : public Instruction {
public:
    SubtractInstruction(std::string dest, Operand lhs, Operand rhs);

    void execute(Process& process) override;
    InstructionType type() const override { return InstructionType::SUBTRACT; }
    std::string toString() const override;

private:
    std::string dest_;
    Operand lhs_;
    Operand rhs_;
};