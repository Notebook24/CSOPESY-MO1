#pragma once
#include <string>
#include <vector>

// Owner: Alain
class StringUtils {
public:
    // Removes spaces, tabs and newlines from both ends.
    static std::string trim(const std::string& text);

    // Returns a lowercase copy.
    static std::string toLower(const std::string& text);

    // Splits on any whitespace and skips empty pieces.
    // "screen -s  p1" -> {"screen", "-s", "p1"}
    static std::vector<std::string> splitWords(const std::string& text);

    // Pads with leading zeros up to a minimum width.
    // padNumber(3, 2) -> "03", padNumber(120, 2) -> "120"
    static std::string padNumber(int value, int width);
};
