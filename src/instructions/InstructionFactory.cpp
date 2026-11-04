#include "instructions/InstructionFactory.h"

#include <random>

#include "instructions/AddInstruction.h"
#include "instructions/DeclareInstruction.h"
#include "instructions/ForInstruction.h"
#include "instructions/PrintInstruction.h"
#include "instructions/SleepInstruction.h"
#include "instructions/SubtractInstruction.h"

namespace {

std::mt19937& rng() {
    thread_local std::mt19937 gen{std::random_device{}()};
    return gen;
}

int randInt(int lo, int hi) { // inclusive
    return std::uniform_int_distribution<int>(lo, hi)(rng());
}

const char* const VAR_POOL[] = {"x", "y", "z", "a", "b", "c"};
constexpr int VAR_POOL_SIZE = 6;

std::string randomVar() { return VAR_POOL[randInt(0, VAR_POOL_SIZE - 1)]; }

Operand randomOperand() {
    if (randInt(0, 1) == 0) return Operand::variable(randomVar());
    return Operand::literal(static_cast<uint16_t>(randInt(0, 1000)));
}

} // namespace

InstructionPtr InstructionFactory::createRandomSimple(const std::string& processName) {
    int roll = randInt(1, 100);
    if (roll <= 30) {
        return std::make_shared<PrintInstruction>("Hello world from " + processName + "!");
    } else if (roll <= 45) {
        return std::make_shared<DeclareInstruction>(randomVar(),
                                                    static_cast<uint16_t>(randInt(0, 1000)));
    } else if (roll <= 65) {
        return std::make_shared<AddInstruction>(randomVar(), randomOperand(), randomOperand());
    } else if (roll <= 85) {
        return std::make_shared<SubtractInstruction>(randomVar(), randomOperand(), randomOperand());
    }
    return std::make_shared<SleepInstruction>(static_cast<uint8_t>(randInt(1, 5)));
}

InstructionPtr InstructionFactory::createRandomFor(const std::string& processName,
                                                   std::size_t maxSize, int depth) {
    int repeats = randInt(2, static_cast<int>(std::min<std::size_t>(5, maxSize)));
    std::size_t bodyBudget = maxSize / repeats; // >= 1 because repeats <= maxSize
    std::size_t bodyLeft = static_cast<std::size_t>(randInt(1, static_cast<int>(bodyBudget)));

    std::vector<InstructionPtr> body;
    while (bodyLeft > 0) {
        if (depth < MAX_FOR_DEPTH && bodyLeft >= 2 && randInt(1, 100) <= 20) {
            InstructionPtr nested = createRandomFor(processName, bodyLeft, depth + 1);
            bodyLeft -= nested->unrolledSize();
            body.push_back(nested);
        } else {
            body.push_back(createRandomSimple(processName));
            --bodyLeft;
        }
    }
    return std::make_shared<ForInstruction>(std::move(body), static_cast<uint32_t>(repeats));
}

std::vector<InstructionPtr> InstructionFactory::generateProgram(const std::string& processName,
                                                                uint32_t targetCount) {
    std::vector<InstructionPtr> program;
    std::size_t remaining = targetCount;
    while (remaining > 0) {
        if (remaining >= 2 && randInt(1, 100) <= 15) {
            InstructionPtr loop = createRandomFor(processName, remaining, 1);
            remaining -= loop->unrolledSize();
            program.push_back(loop);
        } else {
            program.push_back(createRandomSimple(processName));
            --remaining;
        }
    }
    return program;
}

std::vector<InstructionPtr> InstructionFactory::flatten(const std::vector<InstructionPtr>& program) {
    std::vector<InstructionPtr> flat;
    for (const auto& ins : program) ins->expand(flat);
    return flat;
}