#include "instructions/DeclareInstruction.h"
#include "process/Process.h"

DeclareInstruction::DeclareInstruction(std::string variable, uint16_t value)
    : variable_(std::move(variable)), value_(value) {}

void DeclareInstruction::execute(Process& process) {
    process.symbols().set(variable_, value_);
}

std::string DeclareInstruction::toString() const {
    return "DECLARE(" + variable_ + ", " + std::to_string(value_) + ")";
}