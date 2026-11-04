#include "instructions/AddInstruction.h"
#include "process/Process.h"

AddInstruction::AddInstruction(std::string dest, Operand lhs, Operand rhs)
    : dest_(std::move(dest)), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

void AddInstruction::execute(Process& process) {
    long long result = static_cast<long long>(lhs_.resolve(process)) + rhs_.resolve(process);
    process.symbols().set(dest_, clampToUint16(result));
}

std::string AddInstruction::toString() const {
    return "ADD(" + dest_ + ", " + lhs_.toString() + ", " + rhs_.toString() + ")";
}