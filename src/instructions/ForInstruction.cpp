#include "instructions/ForInstruction.h"

#include <algorithm>

ForInstruction::ForInstruction(std::vector<InstructionPtr> body, uint32_t repeats)
    : body_(std::move(body)), repeats_(repeats) {}

void ForInstruction::execute(Process& process) {
    for (uint32_t r = 0; r < repeats_; ++r) {
        for (const auto& ins : body_) ins->execute(process);
    }
}

std::string ForInstruction::toString() const {
    std::string s = "FOR([";
    for (size_t i = 0; i < body_.size(); ++i) {
        if (i > 0) s += "; ";
        s += body_[i]->toString();
    }
    return s + "], " + std::to_string(repeats_) + ")";
}

void ForInstruction::expand(std::vector<InstructionPtr>& out) const {
    for (uint32_t r = 0; r < repeats_; ++r) {
        for (const auto& ins : body_) ins->expand(out);
    }
}

std::size_t ForInstruction::unrolledSize() const {
    std::size_t bodySize = 0;
    for (const auto& ins : body_) bodySize += ins->unrolledSize();
    return bodySize * repeats_;
}

int ForInstruction::nestingDepth() const {
    int deepest = 0;
    for (const auto& ins : body_) deepest = std::max(deepest, ins->nestingDepth());
    return 1 + deepest;
}