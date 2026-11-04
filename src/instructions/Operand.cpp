#include "instructions/Operand.h"
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
    return process.getSymbols().get(name);
}

std::string Operand::toString() const {
    return isVariable ? name : std::to_string(value);
}