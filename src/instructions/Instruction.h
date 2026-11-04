#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

/*
 * Interface these instructions expect from process/Process.h (adapt names if yours differ):
 *
 *   const std::string& Process::getName() const;
 *   SymbolTable&       Process::symbols();               // per-process variables
 *   void               Process::addLog(const std::string& msg);   // stamps time + current core
 *   void               Process::sleepFor(uint8_t ticks);  // mark SLEEPING, relinquish CPU
 *
 * and from process/SymbolTable.h:
 *
 *   uint16_t SymbolTable::get(const std::string& name);  // auto-declares as 0 if missing
 *   void     SymbolTable::set(const std::string& name, uint16_t value);
 */
class Process;
class Instruction;
using InstructionPtr = std::shared_ptr<Instruction>;

constexpr uint16_t MAX_UINT16 = 65535;

// uint16 variables are clamped to [0, 65535].
inline uint16_t clampToUint16(long long v) {
    if (v < 0) return 0;
    if (v > MAX_UINT16) return MAX_UINT16;
    return static_cast<uint16_t>(v);
}

enum class InstructionType { PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR };

// An argument that is either a variable name or a uint16 literal.
struct Operand {
    bool isVariable = false;
    std::string name;
    uint16_t value = 0;

    static Operand variable(const std::string& varName);
    static Operand literal(uint16_t v);

    // Variables that don't exist yet are auto-declared as 0.
    uint16_t resolve(Process& process) const;
    std::string toString() const;
};

class Instruction : public std::enable_shared_from_this<Instruction> {
public:
    virtual ~Instruction() = default;

    // Executes ONE step on the process.
    virtual void execute(Process& process) = 0;

    virtual InstructionType type() const = 0;
    virtual std::string toString() const = 0;

    // Appends the flat (loop-unrolled) form of this instruction to `out`.
    // Simple instructions append themselves; FOR appends its body repeated.
    // Instructions must be created with std::make_shared for this to work.
    virtual void expand(std::vector<InstructionPtr>& out) const;

    // Number of flat instructions this expands to (a FOR counts its unrolled body).
    virtual std::size_t unrolledSize() const { return 1; }

    // FOR nesting depth contained in this instruction (0 for simple instructions).
    virtual int nestingDepth() const { return 0; }
};