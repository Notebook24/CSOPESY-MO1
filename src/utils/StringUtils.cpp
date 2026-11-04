#include "utils/StringUtils.h"

#include <algorithm>
#include <cctype>
#include <exception>

namespace StringUtils {

std::string trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) ++start;
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;
    return s.substr(start, end - start);
}

std::string toLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return out;
}

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> tokens;
    std::string current;
    for (char c : s) {
        if (c == delim) {
            if (!current.empty()) { tokens.push_back(current); current.clear(); }
        } else {
            current.push_back(c);
        }
    }
    if (!current.empty()) tokens.push_back(current);
    return tokens;
}

std::string join(const std::vector<std::string>& parts, const std::string& sep) {
    std::string out;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) out += sep;
        out += parts[i];
    }
    return out;
}

bool startsWith(const std::string& s, const std::string& prefix) {
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

std::string stripQuotes(const std::string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"') return s.substr(1, s.size() - 2);
    return s;
}

bool tryParseUInt(const std::string& s, unsigned long long& out) {
    if (s.empty() || s.size() > 20) return false;
    for (char c : s) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    }
    try {
        out = std::stoull(s);
    } catch (const std::exception&) {
        return false; // overflow
    }
    return true;
}

std::string padNumber(unsigned long long n, int width) {
    std::string digits = std::to_string(n);
    if (static_cast<int>(digits.size()) < width) {
        digits.insert(0, width - digits.size(), '0');
    }
    return digits;
}

} // namespace StringUtils