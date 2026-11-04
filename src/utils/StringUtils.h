#pragma once
#include <string>
#include <vector>

namespace StringUtils {

// Removes leading/trailing whitespace.
std::string trim(const std::string& s);

// Lowercases ASCII characters.
std::string toLower(const std::string& s);

// Splits on a delimiter. Empty tokens are skipped ("a  b" -> {"a","b"}).
std::vector<std::string> split(const std::string& s, char delim = ' ');

// Joins tokens using a separator.
std::string join(const std::vector<std::string>& parts, const std::string& sep);

bool startsWith(const std::string& s, const std::string& prefix);

// Removes one pair of surrounding double quotes: "\"rr\"" -> "rr".
std::string stripQuotes(const std::string& s);

// Strict unsigned parse: digits only, no sign, no overflow. Returns false on failure.
bool tryParseUInt(const std::string& s, unsigned long long& out);

// Zero-pads a number: padNumber(7, 2) -> "07" (useful for p01, p02, ...).
std::string padNumber(unsigned long long n, int width);

} // namespace StringUtils