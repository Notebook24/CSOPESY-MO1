#include "console/CommandParser.h"

#include <algorithm>
#include <cctype>
#include <sstream>

std::string CommandParser::lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

ParsedCommand CommandParser::parse(const std::string& line) {
    ParsedCommand cmd;
    std::istringstream in(line);
    std::string token;
    if (!(in >> token)) return cmd;
    cmd.name = lower(token);
    while (in >> token) cmd.args.push_back(token);
    return cmd;
}
