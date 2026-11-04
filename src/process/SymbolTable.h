#pragma once
<<<<<<< HEAD

#include <cstdint>
#include <map>
#include <string> 

class SymbolTable {
    public: 
        static constexpr uint16_t MAX_VALUE = 65535;

        static uint16_t clamp(int64_t value);

        void declare(const std::string& name, int64_t value);

        uint16_t get(const std::string& name);

        void set(const std::string& name, int64_t value);

        bool exists(const std::string& name) const;
        size_t size() const;
        const std::map<std::string, uint16_t>& all() const;

    private:
        std::map<std::string, uint16_t> vars_;
};
=======
#include <cstdint>
#include <map>
#include <string>

// Owner: Trisha
// Per-process variables. Every value is a uint16 clamped to 0..65535.
// Only the scheduler thread touches this while the process runs,
// so it has no lock.
class SymbolTable {
public:
    bool has(const std::string& name) const;

    // Creates or overwrites. The value is clamped first.
    void declare(const std::string& name, long long value);

    // Returns the value. If the variable does not exist yet,
    // it is declared with 0 and 0 is returned (auto-declare rule).
    uint16_t get(const std::string& name);

    // Same as declare(). Separate name so ADD/SUBTRACT code reads clearly.
    void set(const std::string& name, long long value);

    // below 0 -> 0, above 65535 -> 65535
    static uint16_t clamp(long long value);

private:
    std::map<std::string, uint16_t> variables;
};
>>>>>>> origin/main
