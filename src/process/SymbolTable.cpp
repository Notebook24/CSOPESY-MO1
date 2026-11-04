#include "SymbolTable.h"

uint16_t SymbolTable::clamp(int64_t value) {
    if (value < 0)
        return 0;

    if (value > MAX_VALUE)
        return MAX_VALUE;

    return static_cast<uint16_t>(value);
}

void SymbolTable::declare(const std::string& name, int64_t value) {
    vars_[name] = clamp(value);
}

uint16_t SymbolTable::get(const std::string& name) {
    auto it = vars_.find(name);
    if (it == vars_.end()) {
        vars_[name] = 0;
        
        return 0;
    }

    return it->second;
}

void SymbolTable::set(const std::string& name, int64_t value) {
    vars_[name] = clamp(value);
}

bool SymbolTable::exists(const std::string& name) const {
    return vars_.count(name) > 0;
}

size_t SymbolTable::size() const {
    return vars_.size();
}

const std::map<std::string, uint16_t>& SymbolTable::all() const {
    return vars_;
}