#pragma once
#include <string>
#include <vector>

struct ParsedCommand {
    std::string name;                // lowercased command word
    std::vector<std::string> args;   // remaining tokens, original case
    bool empty() const { return name.empty(); }
};

class CommandParser {
public:
    // Splits on any whitespace, so extra spaces/tabs are harmless.
    static ParsedCommand parse(const std::string& line);
    static std::string lower(std::string s);
};
