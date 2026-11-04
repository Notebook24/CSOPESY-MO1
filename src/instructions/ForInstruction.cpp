#include "instructions/ForInstruction.h"
#include "process/Process.h"

ForInstruction::ForInstruction(const std::vector<std::shared_ptr<Instruction>>& body, int repeats)
    : body(body), repeats(repeats < 0 ? 0 : repeats) {}

std::vector<std::shared_ptr<Instruction>> ForInstruction::expand() const {
    std::vector<std::shared_ptr<Instruction>> out;
    for (int r = 0; r < repeats; ++r) {
        for (const auto& child : body) {
            auto nested = std::dynamic_pointer_cast<ForInstruction>(child);
            if (nested) {
                std::vector<std::shared_ptr<Instruction>> inner = nested->expand();
                out.insert(out.end(), inner.begin(), inner.end());
            } else {
                out.push_back(child);
            }
        }
    }
    return out;
}

void ForInstruction::execute(Process& process) {
    for (int r = 0; r < repeats; ++r) {
        for (const auto& child : body) child->execute(process);
    }
}

std::string ForInstruction::toString() const {
    std::string s = "FOR([";
    for (size_t i = 0; i < body.size(); ++i) {
        if (i > 0) s += "; ";
        s += body[i]->toString();
    }
    return s + "], " + std::to_string(repeats) + ")";
}