#pragma once
#include "instructions/Instruction.h"

// FOR([instructions], repeats) - nestable (max 3 levels, enforced by InstructionFactory).
//
// Recommended use: Process stores the FLATTENED program (InstructionFactory::flatten),
// so each CPU tick executes exactly one simple instruction. That keeps the instruction
// pointer, "Current instruction line / Lines of code", SLEEP, and RR preemption simple.
// execute() below is only a fallback that runs the whole loop in a single call.
class ForInstruction : public Instruction {
public:
    ForInstruction(std::vector<InstructionPtr> body, uint32_t repeats);

    void execute(Process& process) override;
    InstructionType type() const override { return InstructionType::FOR; }
    std::string toString() const override;

    void expand(std::vector<InstructionPtr>& out) const override;
    std::size_t unrolledSize() const override;
    int nestingDepth() const override;

    uint32_t repeats() const { return repeats_; }
    const std::vector<InstructionPtr>& body() const { return body_; }

private:
    std::vector<InstructionPtr> body_;
    uint32_t repeats_;
};