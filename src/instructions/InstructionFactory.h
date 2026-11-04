#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "instructions/Instruction.h"

// Owner: Alain
class InstructionFactory {
public:
    // Builds a random program for one process.
    // - PRINT text is always "Hello world from <processName>!"
    // - The returned list is already FLAT (FOR loops expanded).
    // - Its length should land between minIns and maxIns.
    static std::vector<std::shared_ptr<Instruction>> generateProgram(
        const std::string& processName, uint64_t minIns, uint64_t maxIns);
};
