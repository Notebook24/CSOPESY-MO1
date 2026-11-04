#include "instructions/InstructionFactory.h"

#include <algorithm>
#include <random>

#include "instructions/AddInstruction.h"
#include "instructions/DeclareInstruction.h"
#include "instructions/ForInstruction.h"
#include "instructions/Operand.h"
#include "instructions/PrintInstruction.h"
#include "instructions/SleepInstruction.h"
#include "instructions/SubtractInstruction.h"

namespace {

constexpr int MAX_FOR_DEPTH = 3; // spec: FOR can nest up to 3 times

std::mt19937_64& rng() {
    thread_local std::mt19937_64 gen{std::random_device{}()};
    return gen;
}

int randInt(int lo, int hi) { // inclusive
    return std::uniform_int_distribution<int>(lo, hi)(rng());
}

const char* const VAR_POOL[] = {"x", "y", "z", "a", "b", "c"};

std::string randomVar() { return VAR_POOL[randInt(0, 5)]; }

Operand randomOperand() {
    if (randInt(0, 1) == 0) return Operand::variable(randomVar());
    return Operand::literal(static_cast<uint16_t>(randInt(0, 1000)));
}

std::shared_ptr<Instruction> randomSimple(const std::string& processName) {
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

struct BuiltFor {
    std::shared_ptr<ForInstruction> loop;
    std::size_t size; // flat (unrolled) size
};

// Builds a FOR whose flat size is <= maxSize. Requires maxSize >= 2.
BuiltFor randomFor(const std::string& processName, std::size_t maxSize, int depth) {
    int repeats = randInt(2, static_cast<int>(std::min<std::size_t>(5, maxSize)));
    std::size_t bodyBudget = maxSize / static_cast<std::size_t>(repeats); // >= 1
    std::size_t bodySize = static_cast<std::size_t>(randInt(1, static_cast<int>(bodyBudget)));
    std::size_t bodyLeft = bodySize;

    std::vector<std::shared_ptr<Instruction>> body;
    while (bodyLeft > 0) {
        if (depth < MAX_FOR_DEPTH && bodyLeft >= 2 && randInt(1, 100) <= 20) {
            BuiltFor nested = randomFor(processName, bodyLeft, depth + 1);
            bodyLeft -= nested.size;
            body.push_back(nested.loop);
        } else {
            body.push_back(randomSimple(processName));
            --bodyLeft;
        }
    }
    return { std::make_shared<ForInstruction>(body, repeats),
             bodySize * static_cast<std::size_t>(repeats) };
}

} // namespace

std::vector<std::shared_ptr<Instruction>> InstructionFactory::generateProgram(
    const std::string& processName, uint64_t minIns, uint64_t maxIns) {
    if (minIns == 0) minIns = 1;
    if (maxIns < minIns) maxIns = minIns;
    uint64_t target = std::uniform_int_distribution<uint64_t>(minIns, maxIns)(rng());

    // Result is FLAT: FOR loops are expanded here, so Process only sees simple steps.
    std::vector<std::shared_ptr<Instruction>> program;
    uint64_t remaining = target;
    while (remaining > 0) {
        if (remaining >= 2 && randInt(1, 100) <= 15) {
            BuiltFor built = randomFor(processName, static_cast<std::size_t>(remaining), 1);
            std::vector<std::shared_ptr<Instruction>> flat = built.loop->expand();
            program.insert(program.end(), flat.begin(), flat.end());
            remaining -= built.size;
        } else {
            program.push_back(randomSimple(processName));
            --remaining;
        }
    }
    return program;
}