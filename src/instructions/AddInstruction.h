#pragma once
#include <string>
#include "instructions/Instruction.h"
#include "instructions/Operand.h"

// Owner: Alain
// ADD(var1, var2/value, var3/value) -> var1 = var2 + var3 (clamped to 65535)
class AddInstruction : public Instruction {
public:
    AddInstruction(std::string dest, Operand lhs, Operand rhs);

    void execute(Process& process) override;
    std::string toString() const override;

private:
    std::string dest_;
    Operand lhs_;
    Operand rhs_;
};