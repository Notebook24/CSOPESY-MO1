#pragma once
#include <cstdint>
#include <string>

class Process;

constexpr uint16_t MAX_UINT16 = 65535;

// uint16 variables are clamped to [0, 65535].
inline uint16_t clampToUint16(long long v) {
    if (v < 0) return 0;
    if (v > MAX_UINT16) return MAX_UINT16;
    return static_cast<uint16_t>(v);
}

// An ADD/SUBTRACT argument: either a variable name or a uint16 literal.
struct Operand {
    bool isVariable = false;
    std::string name;
    uint16_t value = 0;

    static Operand variable(const std::string& varName);
    static Operand literal(uint16_t v);

    // Missing variables are auto-declared as 0 by the symbol table.
    uint16_t resolve(Process& process) const;
    std::string toString() const;
};