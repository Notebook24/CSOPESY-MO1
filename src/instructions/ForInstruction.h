#pragma once
#include <memory>
#include <vector>
#include "instructions/Instruction.h"

// Owner: Alain
// FOR([instructions], repeats). Can nest up to 3 levels.
//
// The scheduler never runs a ForInstruction directly. The factory calls
// expand() so the Process only ever receives a FLAT list. That way every
// instruction costs exactly one step and the instruction pointer stays simple.
class ForInstruction : public Instruction {
public:
    ForInstruction(const std::vector<std::shared_ptr<Instruction>>& body, int repeats);

    // Returns body repeated 'repeats' times, with any nested FORs also expanded.
    std::vector<std::shared_ptr<Instruction>> expand() const;

    // Runs the whole loop at once. Only for tests, not used by the scheduler.
    void execute(Process& process) override;
    std::string toString() const override;

private:
    std::vector<std::shared_ptr<Instruction>> body;
    int repeats;
};
