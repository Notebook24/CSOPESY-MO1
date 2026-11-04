#pragma once
#include "instructions/Instruction.h"

// PRINT("msg") or PRINT("msg" + var)
class PrintInstruction : public Instruction {
public:
    explicit PrintInstruction(std::string message, std::string variable = "");

    void execute(Process& process) override;
    InstructionType type() const override { return InstructionType::PRINT; }
    std::string toString() const override;

private:
    std::string message_;
    std::string variable_; // empty = no variable appended
};