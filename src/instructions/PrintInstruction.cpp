#include "instructions/PrintInstruction.h"
#include "process/Process.h"

PrintInstruction::PrintInstruction(std::string message, std::string variable)
    : message_(std::move(message)), variable_(std::move(variable)) {}

void PrintInstruction::execute(Process& process) {
    std::string out = message_;
    if (!variable_.empty()) {
        out += std::to_string(process.getSymbols().get(variable_));
    }
    process.addLog(out);
}

std::string PrintInstruction::toString() const {
    std::string s = "PRINT(\"" + message_ + "\"";
    if (!variable_.empty()) s += " + " + variable_;
    return s + ")";
}