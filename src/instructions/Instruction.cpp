#include "instructions/Instruction.h"
#include "process/Process.h"

Operand Operand::variable(const std::string& varName) {
    Operand o;
    o.isVariable = true;
    o.name = varName;
    return o;
}

Operand Operand::literal(uint16_t v) {
    Operand o;
    o.isVariable = false;
    o.value = v;
    return o;
}

uint16_t Operand::resolve(Process& process) const {
    if (!isVariable) return value;
    return process.symbols().get(name);
}

std::string Operand::toString() const {
    return isVariable ? name : std::to_string(value);
}

void Instruction::expand(std::vector<InstructionPtr>& out) const {
    out.push_back(std::const_pointer_cast<Instruction>(shared_from_this()));
}