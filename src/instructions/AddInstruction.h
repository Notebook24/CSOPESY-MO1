#pragma once
#include "instructions/Instruction.h"

// ADD(var1, var2/value, var3/value) -> var1 = var2 + var3 (clamped to uint16 max).
class AddInstruction : public Instruction {
public:
    AddInstruction(std::string dest, Operand lhs, Operand rhs);

    void execute(Process& process) override;
    InstructionType type() const override { return InstructionType::ADD; }
    std::string toString() const override;

private:
    std::string dest_;
    Operand lhs_;
    Operand rhs_;
};