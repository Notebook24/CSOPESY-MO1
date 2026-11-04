#pragma once

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