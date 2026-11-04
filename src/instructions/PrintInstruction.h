#pragma once
#include <string>
#include "instructions/Instruction.h"

// Owner: Alain
// PRINT("msg") or PRINT("msg" + var)
class PrintInstruction : public Instruction {
public:
    explicit PrintInstruction(std::string message, std::string variable = "");

    void execute(Process& process) override;
    std::string toString() const override;

private:
    std::string message_;
    std::string variable_; // empty = nothing appended
};