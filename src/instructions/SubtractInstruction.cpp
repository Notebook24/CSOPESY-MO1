#include "instructions/SubtractInstruction.h"
#include "process/Process.h"

SubtractInstruction::SubtractInstruction(std::string dest, Operand lhs, Operand rhs)
    : dest_(std::move(dest)), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

void SubtractInstruction::execute(Process& process) {
    long long result = static_cast<long long>(lhs_.resolve(process)) - rhs_.resolve(process);
    process.getSymbols().set(dest_, clampToUint16(result));
}

std::string SubtractInstruction::toString() const {
    return "SUBTRACT(" + dest_ + ", " + lhs_.toString() + ", " + rhs_.toString() + ")";
}