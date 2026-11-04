#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "instructions/Instruction.h"

// Generates the randomized, pre-determined programs for processes made by
// scheduler-start (and screen -s). Users never type instructions.
class InstructionFactory {
public:
    static constexpr int MAX_FOR_DEPTH = 3;

    // Builds a program whose UNROLLED length is exactly `targetCount`
    // (pick targetCount in [min-ins, max-ins] from config).
    // Top-level result may contain FOR instructions (nested up to 3 levels).
    static std::vector<InstructionPtr> generateProgram(const std::string& processName,
                                                       uint32_t targetCount);

    // Unrolls FOR loops into a flat list: one entry per executable step.
    static std::vector<InstructionPtr> flatten(const std::vector<InstructionPtr>& program);

    // One random non-FOR instruction. PRINT's message is always
    // "Hello world from <processName>!".
    static InstructionPtr createRandomSimple(const std::string& processName);

private:
    // FOR whose unrolled size is <= maxSize (requires maxSize >= 2).
    static InstructionPtr createRandomFor(const std::string& processName,
                                          std::size_t maxSize, int depth);
};