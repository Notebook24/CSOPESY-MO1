#include "utils/StringUtils.h"

#include <algorithm>
#include <cctype>

std::string StringUtils::trim(const std::string& text) {
    size_t start = 0;
    while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start]))) ++start;
    size_t end = text.size();
    while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1]))) --end;
    return text.substr(start, end - start);
}

std::string StringUtils::toLower(const std::string& text) {
    std::string out = text;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return out;
}

std::vector<std::string> StringUtils::splitWords(const std::string& text) {
    std::vector<std::string> words;
    std::string current;
    for (char c : text) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!current.empty()) { words.push_back(current); current.clear(); }
        } else {
            current.push_back(c);
        }
    }
    if (!current.empty()) words.push_back(current);
    return words;
}

std::string StringUtils::padNumber(int value, int width) {
    std::string digits = std::to_string(value);
    if (static_cast<int>(digits.size()) < width) {
        digits.insert(0, width - digits.size(), '0');
    }
    return digits;
}