#pragma once
#include <string>

// Owner: Alain
class Timestamp {
public:
    // Current local time as "MM/DD/YYYY HH:MM:SSAM" (or PM).
    // Example: "10/06/2025 02:15:30PM"
    // No parentheses. Callers add "(" and ")" when they need them.
    static std::string now();
};
